#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <json-c/json_object.h>
#include <readline/readline.h>


std::vector<std::string> split_command(const std::string& input) {
    std::vector<std::string> command;
    std::stringstream argument_stream(input);
    std::string arg; // temporary storage for argument parsing

    while (std::getline(argument_stream, arg, ' ')) {
        if (arg.empty()) continue;

        command.push_back(arg);
    }

    return command;
}


std::shared_ptr<json_object> create_command_json(const std::vector<std::string>& command) {
    // custom deallocator for freeing resources properly
    auto json = std::shared_ptr<json_object>(json_object_new_object(), json_object_put);
    auto argument_array = json_object_new_array();

    auto it = command.begin();

    // get command part
    json_object_object_add(json.get(), "command", json_object_new_string(it++->data()));

    // get arguments one by one
    while (it != command.end()) {
        json_object_array_add(argument_array, json_object_new_string(it++->data()));
    }

    json_object_object_add(json.get(), "arguments", argument_array);

    return json;
}


int main(int argc, char** argv) {
    std::vector<std::shared_ptr<json_object>> commands;

    while (true) {
        auto input = std::shared_ptr<char>(readline(""));
        auto input_string = std::string(input.get());
        if (input_string == "EXIT") break;

        auto command = split_command(input_string);
        if (command.empty()) continue;
        commands.push_back(create_command_json(command));
    }

    for (auto&& command : commands) {
        auto out = json_object_to_json_string_ext(command.get(), JSON_C_TO_STRING_PRETTY);
        std::cout << out << std::endl;
    }

    return 0;
}
