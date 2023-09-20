"""The unittest.TestCase to inject multiple cluster connection strings into TestData."""

import hashlib
import threading

from buildscripts.resmokelib.testing.testcases import interface
from buildscripts.resmokelib.testing.testcases import jsrunnerfile


class BulkWriteClusterTestCase(jsrunnerfile.JSRunnerFileTestCase):
    """An test to execute with connection data for multiple clusters passed through TestData."""

    REGISTERED_NAME = "bulk_write_cluster_js_test"

    def __init__(self, logger, js_filename, shell_executable=None, shell_options=None):
        """Initialize the BulkWriteClusterTestCase."""
        jsrunnerfile.JSRunnerFileTestCase.__init__(
            self, logger, "BulkWriteCluster Test", js_filename,
            test_runner_file="jstests/libs/bulk_write_passthrough_runner.js",
            shell_executable=shell_executable, shell_options=shell_options)

    @property
    def js_filename(self):
        """Return the name of the test file."""
        return self.test_name

    def _populate_test_data(self, test_data):
        test_data["jsTestFile"] = self.js_filename
        test_data["bulkWriteCluster"] = self.fixture.clusters[0].get_driver_connection_url()
        test_data["normalCluster"] = self.fixture.clusters[1].get_driver_connection_url()
