# Copyright 2016 Google Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
################################################################################
#
workspace(name = "io_istio_proxy")

# http_archive is not a native function since bazel 0.19
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

# 1. Determine SHA256 `wget https://github.com/envoyproxy/envoy/archive/$COMMIT.tar.gz && sha256sum $COMMIT.tar.gz`
# 2. Update .bazelversion, envoy.bazelrc and .bazelrc if needed.
#
# Commit date: 2024-08-17
ENVOY_SHA = "dbb35fa0d4b61f29c6ae81aca2cbd88184207d18"

ENVOY_SHA256 = "90816a929ff92560e4e19deb2e5cf54bc82cc998cd67c13a73a614205257b5b1"

ENVOY_ORG = "envoyproxy"

ENVOY_REPO = "envoy"

# To override with local envoy, just pass `--override_repository=envoy=/PATH/TO/ENVOY` to Bazel or
# persist the option in `user.bazelrc`.
http_archive(
    name = "envoy",
    sha256 = ENVOY_SHA256,
    strip_prefix = ENVOY_REPO + "-" + ENVOY_SHA,
    url = "https://github.com/" + ENVOY_ORG + "/" + ENVOY_REPO + "/archive/" + ENVOY_SHA + ".tar.gz",
)

http_archive(
    name = "json_schema_validator",
    urls = ["https://github.com/pboettch/json-schema-validator/archive/refs/tags/2.1.0.tar.gz"],
    strip_prefix = "json-schema-validator-2.1.0",
    build_file_content = """
cc_library(
    name = "json_schema_validator",
    srcs = glob(["src/*.cpp"]),
    hdrs = glob(["src/*.hpp", "src/*.h", "src/nlohmann/*.hpp"]),
    includes = ["src"],
    deps = ["@nlohmann_json//:json"],
    visibility = ["//visibility:public"],
)
    """,
)

http_archive(
    name = "nlohmann_json",
    urls = ["https://github.com/nlohmann/json/archive/refs/tags/v3.11.2.tar.gz"],
    strip_prefix = "json-3.11.2",
    build_file_content = """
cc_library(
    name = "json",
    hdrs = glob(["single_include/nlohmann/*.hpp"]),
    includes = ["single_include"],
    visibility = ["//visibility:public"],
)
    """,
)

http_archive(
    name = "com_github_cncf_xds",
    urls = ["https://github.com/cncf/xds/archive/555b57ec207be86f811fb0c04752db6f85e3d7e2.tar.gz"],
    strip_prefix = "xds-555b57ec207be86f811fb0c04752db6f85e3d7e2",
    sha256 = "0c8c4f0f67fed967b51049f7d5e2ca7a9bd433970a29c88e272c8665328172f5",
)

load("@envoy//bazel:api_binding.bzl", "envoy_api_binding")

local_repository(
    name = "envoy_build_config",
    # Relative paths are also supported.
    path = "bazel/extension_config",
)

envoy_api_binding()

load("@envoy//bazel:api_repositories.bzl", "envoy_api_dependencies")

envoy_api_dependencies()

load("@envoy//bazel:repositories.bzl", "envoy_dependencies")

envoy_dependencies()

load("@envoy//bazel:repositories_extra.bzl", "envoy_dependencies_extra")

envoy_dependencies_extra(ignore_root_user_error = True)

load("@envoy//bazel:python_dependencies.bzl", "envoy_python_dependencies")

envoy_python_dependencies()

load("@base_pip3//:requirements.bzl", "install_deps")

install_deps()

load("@envoy//bazel:dependency_imports.bzl", "envoy_dependency_imports")

envoy_dependency_imports()
