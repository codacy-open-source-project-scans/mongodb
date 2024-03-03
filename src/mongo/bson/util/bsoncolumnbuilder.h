/**
 *    Copyright (C) 2021-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#pragma once

#include <boost/move/utility_core.hpp>
#include <boost/optional/optional.hpp>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

#include "mongo/bson/bsonelement.h"
#include "mongo/bson/bsonelementvalue.h"
#include "mongo/bson/bsonmisc.h"
#include "mongo/bson/bsonobj.h"
#include "mongo/bson/bsontypes.h"
#include "mongo/bson/bsontypes_util.h"
#include "mongo/bson/util/builder.h"
#include "mongo/bson/util/simple8b_builder.h"
#include "mongo/platform/int128.h"

namespace mongo {
namespace bsoncolumn {
/**
 * Deconstructed BSONElement without type and fieldname in the contigous buffer.
 */
struct Element {
    Element() : type(EOO), size(0) {}
    Element(BSONElement elem) : value(elem.value()), type(elem.type()), size(elem.valuesize()) {}
    Element(const BSONObj& obj, BSONType t) : value(obj.objdata()), type(t), size(obj.objsize()) {}
    Element(BSONType t, BSONElementValue v, int s) : value(v), type(t), size(s) {}

    // Performs binary memory compare
    bool operator==(const Element& rhs) const;

    BSONElementValue value;
    BSONType type;
    int size;
};

/**
 * State for encoding scalar BSONElement as BSONColumn using delta or delta-of-delta
 * compression. When compressing Objects one Encoding state is used per sub-field within the
 * object to compress.
 */
struct EncodingState {
    struct Encoder64;
    struct Encoder128;

    template <class F>
    class Simple8bBlockWriter128 {
    public:
        Simple8bBlockWriter128(BufBuilder& buffer,
                               ptrdiff_t& controlByteOffset,
                               F controlBlockWriter)
            : _buffer(buffer),
              _controlByteOffset(controlByteOffset),
              _controlBlockWriter(std::move(controlBlockWriter)) {}

        void operator()(uint64_t block);

    private:
        BufBuilder& _buffer;
        ptrdiff_t& _controlByteOffset;
        F _controlBlockWriter;
    };

    template <class F>
    class Simple8bBlockWriter64 {
    public:
        Simple8bBlockWriter64(Encoder64& encoder,
                              BufBuilder& buffer,
                              ptrdiff_t& controlByteOffset,
                              BSONType type,
                              F controlBlockWriter)
            : _encoder(encoder),
              _buffer(buffer),
              _controlByteOffset(controlByteOffset),
              _type(type),
              _controlBlockWriter(std::move(controlBlockWriter)) {}

        void operator()(uint64_t block);

    private:
        Encoder64& _encoder;
        BufBuilder& _buffer;
        ptrdiff_t& _controlByteOffset;
        BSONType _type;
        F _controlBlockWriter;
    };

    struct NoopControlBlockWriter {
        void operator()(ptrdiff_t, size_t) const {}
    };

    // Encoder state for 64bit types
    struct Encoder64 {
        Encoder64();

        // Initializes this encoder to uncompressed element to allow for future delta calculations.
        void initialize(Element elem);

        // Calculates and appends delta for this element compared to last.
        template <class F>
        bool appendDelta(Element elem,
                         Element previous,
                         BufBuilder& buffer,
                         ptrdiff_t& controlByteOffset,
                         F controlBlockWriter);

        // Appends encoded value to simple8b builder
        template <class F>
        bool append(BSONType type,
                    uint64_t value,
                    BufBuilder& buffer,
                    ptrdiff_t& controlByteOffset,
                    F controlBlockWriter);

        // Appends skip to simple8b builder
        template <class F>
        void skip(BSONType type,
                  BufBuilder& buffer,
                  ptrdiff_t& controlByteOffset,
                  F controlBlockWriter);

        // Flushes simple8b builder, causes all pending values to be written as blocks
        template <class F>
        void flush(BSONType type,
                   BufBuilder& buffer,
                   ptrdiff_t& controlByteOffset,
                   F controlBlockWriter);

        // Simple-8b builder for storing compressed deltas
        Simple8bBuilder<uint64_t> simple8bBuilder;
        // Additional variables needed for tracking previous state
        int64_t prevDelta = 0;
        int64_t prevEncoded64 = 0;
        double lastValueInPrevBlock = 0;
        uint8_t scaleIndex;

    private:
        // Helper to append doubles to this Column builder. Returns true if append was successful
        // and false if the value needs to be stored uncompressed.
        template <class F>
        bool _appendDouble(double value,
                           double previous,
                           BufBuilder& buffer,
                           ptrdiff_t& controlByteOffset,
                           F controlBlockWriter);

        // Tries to rescale current pending values + one additional value into a new
        // Simple8bBuilder. Returns the new Simple8bBuilder if rescaling was possible and none
        // otherwise.
        boost::optional<Simple8bBuilder<uint64_t>> _tryRescalePending(int64_t encoded,
                                                                      uint8_t newScaleIndex) const;
    };

    // Encoder state for 128bit types
    struct Encoder128 {
        // Initializes this encoder to uncompressed element to allow for future delta calculations.
        void initialize(Element elem);

        // Calculates and appends delta for this element compared to last.
        template <class F>
        bool appendDelta(Element elem,
                         Element previous,
                         BufBuilder& buffer,
                         ptrdiff_t& controlByteOffset,
                         F controlBlockWriter);

        // Appends encoded value to simple8b builder
        template <class F>
        bool append(BSONType type,
                    uint128_t value,
                    BufBuilder& buffer,
                    ptrdiff_t& controlByteOffset,
                    F controlBlockWriter);

        // Appends skip to simple8b builder
        template <class F>
        void skip(BSONType type,
                  BufBuilder& buffer,
                  ptrdiff_t& controlByteOffset,
                  F controlBlockWriter);

        // Flushes simple8b builder, causes all pending values to be written as blocks
        template <class F>
        void flush(BSONType type,
                   BufBuilder& buffer,
                   ptrdiff_t& controlByteOffset,
                   F controlBlockWriter);

        // Simple-8b builder for storing compressed deltas
        Simple8bBuilder<uint128_t> simple8bBuilder;
        // Additional variables needed for previous state
        boost::optional<int128_t> prevEncoded128;
    };

    EncodingState();

    template <class F>
    void append(Element elem, BufBuilder& buffer, F controlBlockWriter);
    template <class F>
    void skip(BufBuilder& buffer, F controlBlockWriter);
    template <class F>
    void flush(BufBuilder& buffer, F controlBlockWriter);

    template <class Encoder, class F>
    void appendDelta(
        Encoder& encoder, Element elem, Element previous, BufBuilder& buffer, F controlBlockWriter);
    Element _previous() const;
    void _storePrevious(Element elem);
    template <class F>
    void _writeLiteralFromPrevious(BufBuilder& buffer, F controlBlockWriter);
    void _initializeFromPrevious();
    template <class F>
    ptrdiff_t _incrementSimple8bCount(BufBuilder& buffer, F controlBlockWriter);

    /**
     * Copyable memory buffer
     */
    struct CloneableBuffer {
        CloneableBuffer() = default;

        CloneableBuffer(CloneableBuffer&&) = default;
        CloneableBuffer(const CloneableBuffer&);

        CloneableBuffer& operator=(CloneableBuffer&&) = default;
        CloneableBuffer& operator=(const CloneableBuffer&);

        std::unique_ptr<char[]> buffer;
        int size = 0;
        int capacity = 0;
    };

    // Encoders for 64bit and 128bit types.
    std::variant<Encoder64, Encoder128> _encoder;

    // Storage for the previously appended BSONElement
    CloneableBuffer _prev;

    // Offset to last Simple-8b control byte
    std::ptrdiff_t _controlByteOffset;
};
}  // namespace bsoncolumn

/**
 * Class to build BSON Subtype 7 (Column) binaries.
 */
class BSONColumnBuilder {
public:
    BSONColumnBuilder();
    explicit BSONColumnBuilder(BufBuilder builder);

    /**
     * Initializes this BSONColumnBuilder from a BSONColumn binary. Leaves the BSONColumnBuilder in
     * a state as-if the contents of the BSONColumn have been appended to it and intermediate() has
     * been called. This allows for efficient appending of new data to this BSONColumn and
     * calculating binary diffs using intermediate() of this data.
     *
     * finalize() may not be used after this constructor.
     */
    BSONColumnBuilder(const char* binary, int size);

    /**
     * Appends a BSONElement to this BSONColumnBuilder.
     *
     * Value will be stored delta compressed if possible and uncompressed otherwise.
     *
     * The field name will be ignored.
     *
     * EOO is treated as 'skip'.
     *
     * Throws InvalidBSONType if MinKey or MaxKey is appended.
     */
    BSONColumnBuilder& append(BSONElement elem);

    /**
     * Appends a BSONObj to this BSONColumnBuilder.
     *
     * Like appending a BSONElement of type Object.
     */
    BSONColumnBuilder& append(const BSONObj& obj);

    /**
     * Appends a BSONArray to this BSONColumnBuilder.
     *
     * Like appending a BSONElement of type Array.
     */
    BSONColumnBuilder& append(const BSONArray& arr);

    /**
     * Appends an index skip to this BSONColumnBuilder.
     */
    BSONColumnBuilder& skip();

    /**
     * Returns a BSON Column binary diff relative to previous intermediate() call(s). Leaves the
     * BSONColumnBuilder in a state where it is allowed to continue append data to it. Less
     * efficient than producing than 'finalize' when used to produce full binaries.
     *
     * May not be called after finalize() has been called.
     */
    class [[nodiscard]] BinaryDiff {
    public:
        BinaryDiff(SharedBuffer buffer, int bufferSize, int readOffset, int writeOffset)
            : _buffer(std::move(buffer)),
              _bufferSize(bufferSize),
              _readOffset(readOffset),
              _writeOffset(writeOffset) {}

        /**
         * Binary data in this diff to be changed after the offset point.
         *
         * This BinaryDiff must remain in scope for this pointer to be valid.
         */
        const char* data() const {
            return _buffer.get() + _readOffset;
        }

        /**
         * Size of binary data in this diff
         */
        int size() const {
            return _bufferSize - _readOffset;
        }

        /**
         * Absolute location in binaries obtained from previous intermediate() calls where this diff
         * should be applied.
         *
         * Returns 0 the first time intermediate() has been called.
         */
        int offset() const {
            return _writeOffset;
        }

    private:
        SharedBuffer _buffer;
        int _bufferSize;
        int _readOffset;
        int _writeOffset;
    };

    BinaryDiff intermediate();

    /**
     * Finalizes the BSON Column and returns the full BSONColumn binary. Further data append is not
     * allowed.
     *
     * The BSONColumnBuilder must remain in scope for the data to be valid.
     *
     * May not be called after intermediate() or the constructor that initializes the builder from a
     * previous binary.
     */
    BSONBinData finalize();

    /**
     * Detaches the buffer associated with this BSONColumnBuilder. Allows the memory to be reused
     * for building another BSONColumn.
     */
    BufBuilder detach();

    /**
     * Returns the number of interleaved start control bytes this BSONColumnBuilder has written.
     */
    int numInterleavedStartWritten() const;

    /**
     * Validates that the internal state of this BSONColumnBuilder is identical to the provided one.
     * This guarantees that appending more data to either of them would produce the same binary.
     */
    void assertInternalStateIdentical_forTest(const BSONColumnBuilder& other) const;

    /**
     * Returns the last non-skipped appended scalar element into this BSONColumnBuilder.
     *
     * If the builder is not in scalar mode internally, EOO is returned.
     */
    BSONElement last() const;

private:
    /**
     * Internal mode this BSONColumnBuilder is in.
     */
    enum class Mode {
        // Regular mode without interleaving. Appended elements are treated as scalars.
        kRegular,
        // Interleaved mode where the reference object is being determined. New sub fields are
        // attempted to be merged in to the existing reference object candidate.
        kSubObjDeterminingReference,
        // Interleaved mode with a fixed reference object. Any incompatible sub fields in appended
        // objects must exit interleaved mode.
        kSubObjAppending
    };

    /**
     * Internal state of the BSONColumnBuilder. Can be copied to restore a previous state after
     * finalize.
     */
    struct InternalState {
        InternalState();

        Mode mode = Mode::kRegular;

        // Encoding state for kRegular mode
        bsoncolumn::EncodingState regular;

        struct SubObjState {
            // We need to buffer all control blocks written by the EncodingStates
            // so they can be added to the main buffer in the right order.
            class InterleavedControlBlockWriter {
            public:
                InterleavedControlBlockWriter(
                    std::vector<std::pair<ptrdiff_t, size_t>>& controlBlocks);

                void operator()(ptrdiff_t, size_t);

            private:
                std::vector<std::pair<ptrdiff_t, size_t>>& _controlBlocks;
            };

            SubObjState();
            SubObjState(SubObjState&&);
            SubObjState(const SubObjState&);

            SubObjState& operator=(SubObjState&&);
            SubObjState& operator=(const SubObjState&);

            bsoncolumn::EncodingState state;
            BufBuilder buffer;
            std::vector<std::pair<ptrdiff_t, size_t>> controlBlocks;

            InterleavedControlBlockWriter controlBlockWriter();
        };

        // Encoding states when in sub-object compression mode. There should be one encoding state
        // per scalar field in '_referenceSubObj'.
        std::vector<SubObjState> subobjStates;

        // Reference object that is used to match object hierarchy to encoding states. Appending
        // objects for sub-object compression need to check their hierarchy against this object.
        BSONObj referenceSubObj;
        BSONType referenceSubObjType;

        // Buffered BSONObj when determining reference object. Will be compressed when this is
        // complete and we transition into kSubObjAppending.
        std::vector<BSONObj> bufferedObjElements;

        // Helper to flatten Object to compress to match _subobjStates
        std::vector<BSONElement> flattenedAppendedObj;

        // Current offset of the binary relative to previous intermediate() calls.
        int offset = 0;

        // Buffer length at previous intermediate() call.
        int lastBufLength = 0;
        // Finalized state of last control byte written out by the previous intermediate() call.
        uint8_t lastControl;
    };

    // Internal helper to perform reopen/initialization of this class from a BSONColumn binary.
    class BinaryReopen;

    // Append helper for appending a BSONObj
    BSONColumnBuilder& _appendObj(bsoncolumn::Element elem);

    // Append Object for sub-object compression when in mode kSubObjAppending
    bool _appendSubElements(const BSONObj& obj);

    // Transition into kSubObjDeterminingReference mode
    void _startDetermineSubObjReference(const BSONObj& obj, BSONType type);

    // Transition from kSubObjDeterminingReference into kSubObjAppending
    void _finishDetermineSubObjReference();

    // Transition from kSubObjDeterminingReference or kSubObjAppending back into kRegular.
    void _flushSubObjMode();

    InternalState _is;

    // Buffer for the BSON Column binary
    BufBuilder _bufBuilder;

    int _numInterleavedStartWritten = 0;
};

}  // namespace mongo
