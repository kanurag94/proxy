load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
)

package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache 2

envoy_cc_library(
    name = "config_lib",
    srcs = [
        "config.cc",
        "filter.cc",
    ],
    hdrs = [
        "config.h",
        "filter.h",
    ],
    repository = "@envoy",
    deps = [
        ":config_cc_proto",
        "@json_schema_validator//:json_schema_validator",
        "@envoy//envoy/http:filter_interface",
        "@envoy//envoy/runtime:runtime_interface",
        "@envoy//source/common/buffer:buffer_lib",
        "@envoy//source/common/http:codes_lib",
        "@envoy//source/common/http:exception_lib",
        "@envoy//source/extensions/filters/http/common:factory_base_lib",
    ],
)

cc_proto_library(
    name = "config_cc_proto",
    deps = ["config"],
)

proto_library(
    name = "config",
    srcs = ["config.proto"],
)
