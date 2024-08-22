#include <string>

#include "envoy/registry/registry.h"

#include "source/extensions/filters/http/payload_validator/config.h"
#include "source/extensions/filters/http/payload_validator/filter.h"

using nlohmann::json;
using istio::envoy::config::filter::http::payload_validator::v3alpha1::FilterConfig;

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace PayloadValidator {

const std::shared_ptr<PayloadDescription> Operation::getResponseValidator(uint32_t code) const {
  auto it = responses_.find(code);

  if (it == responses_.end()) {
    return nullptr;
  }

  return (*it).second;
}

bool JSONPayloadDescription::initialize(const std::string& schema) {
  // Convert schema string to nlohmann::json object.
  json schema_as_json;
  try {
    schema_as_json = json::parse(schema);
  } catch (...) {
    return false;
  }
  // Schema seems to be a valid json doc, but it does not mean it describes
  // proper json schema.

  active_ = true;
  validator_.set_root_schema(schema_as_json);
  return true;
}

std::pair<bool, absl::optional<std::string>>
JSONPayloadDescription::validate(const Buffer::Instance& data) {
  std::string message;
  message.assign(std::string(
      static_cast<char*>((const_cast<Buffer::Instance&>(data)).linearize(data.length())),
      data.length()));

  // Todo (reject if this is not json).
  json rec_buf;
  try {
    rec_buf = json::parse(message);
  } catch (const std::exception& e) {
    // Payload is not valid JSON.
    return std::make_pair<bool, absl::optional<std::string>>(false,
                                                             absl::optional<std::string>(e.what()));
  }

  try {
    validator_.validate(rec_buf);
    // No error.
  } catch (const std::exception& e) {
    std::cerr << "Payload does not match the schema, here is why: " << e.what() << "\n";
    return std::make_pair<bool, absl::optional<std::string>>(false,
                                                             absl::optional<std::string>(e.what()));
  }

  return std::make_pair<bool, absl::optional<std::string>>(true, std::nullopt);
}

bool FilterConfig::processConfig(
    const envoy::extensions::filters::http::payload_validator::v3::PayloadValidator& config) {
  // bool request_found = false;
  // bool response_found = false;

  stat_prefix_ = config.stat_prefix();

  if (config.operations().empty()) {
    return false;
  }

  // iterate over configured operations.
  for (const auto& operation : config.operations()) {
    // const auto& method = operation.method();
    auto new_operation = std::make_shared<Operation>();

    auto request_validator = std::make_unique<JSONPayloadDescription>();

    if (operation.has_request_max_size()) {
      request_validator->setMaxSize(operation.request_max_size().value());
    }

    if (!operation.request_body().schema().empty()) {

      if (!request_validator->initialize(operation.request_body().schema())) {
        return false;
      }

      //  request_found = true;
    }
    new_operation->request_ = std::move(request_validator);

    // Iterate over response codes and their expected formats.
    for (const auto& response : operation.responses()) {
      auto code = response.http_status().code();

      if (!response.response_body().schema().empty()) {
        auto response_validator = std::make_shared<JSONPayloadDescription>();
        if (!response_validator->initialize(response.response_body().schema())) {
          return false;
        }

        new_operation->responses_.emplace(code, std::move(response_validator));
        //   response_found = true;
      } else {
        new_operation->responses_.emplace(code, nullptr);
      }
    }

    std::string method = envoy::config::core::v3::RequestMethod_Name(operation.method());
    operations_.emplace(method, std::move(new_operation));
  }

  /*
    if (!(request_found || response_found)) {
      return false;
    }
  */

  return true;
}

// Find context related to method.
const std::shared_ptr<Operation> FilterConfig::getOperation(const std::string& name) const {
  const auto it = operations_.find(name);

  if (it == operations_.end()) {

    return nullptr;
  }

  return (*it).second;
}

Http::FilterFactoryCb FilterConfigFactory::createFilterFactoryFromProtoTyped(
    const envoy::extensions::filters::http::payload_validator::v3::PayloadValidator& config,
    const std::string& stats_prefix, Server::Configuration::FactoryContext& context) {

  std::cerr << stats_prefix << "\n";
  std::string final_prefix =
      fmt::format("{}payload_validator.{}", stats_prefix, config.stat_prefix());
  std::shared_ptr<FilterConfig> filter_config =
      std::make_shared<FilterConfig>(final_prefix, context.scope());

  if (!filter_config->processConfig(config)) {
    throw EnvoyException(fmt::format("Invalid payload validator config: {}", "TODO"));
  }

#if 0
  // to-do. Check if schema is a valid json.
  json schema = json::parse((config.schema()));;
  try {
  //validator_.set_root_schema(person_schema);  
  validator_.set_root_schema(schema);  
    } catch (const std::exception &e) {
    std::cerr << "Validation of schema failed, here is why: " << e.what() << "\n";
    }
#endif

  return [filter_config](Http::FilterChainFactoryCallbacks& callbacks) -> void {
    callbacks.addStreamFilter(std::make_shared<Filter>(*filter_config));
  };
}

/**
 * Static registration for the http payload validator filter. @see RegisterFactory.
 */
LEGACY_REGISTER_FACTORY(FilterConfigFactory, Server::Configuration::NamedHttpFilterConfigFactory,
                        "envoy.http_payload_validator_filter");

} // namespace PayloadValidator
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
