/**
 * update_array_noindex.js
 *
 * Executes the update_array.js workload after dropping all non-_id indexes on
 * the collection.
 */
import {extendWorkload} from "jstests/concurrency/fsm_libs/extend_workload.js";
import {dropAllIndexes} from "jstests/concurrency/fsm_workload_modifiers/drop_all_indexes.js";
import {$config as $baseConfig} from "jstests/concurrency/fsm_workloads/update_array.js";

export const $config = extendWorkload($baseConfig, dropAllIndexes);
