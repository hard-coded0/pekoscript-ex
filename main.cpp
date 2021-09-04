#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <streambuf>
#include "tokenizer.hpp"

std::string readFileIntoString(const std::string& path) {
    std::ifstream input_file(path);

    if (!input_file.is_open()) {
        std::cerr << "Could not open the file - '"
            << path << "'" << std::endl;
        exit(EXIT_FAILURE);
    }

    return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());    
}


int main(int argc, char** argv) {
  std::vector<Tokenizer::Token>     tokenss    = Tokenizer::SplitToks(readFileIntoString(std::string(argv[1])));
  std::vector<Tokenizer::Statement> fin_tokens = Tokenizer::FinTokens(tokenss);

  std::map<int, std::string> token_types;

  token_types[-1] = "Function";
  token_types[-2] = "Identifier";
  token_types[-3] = "Number";
  token_types[-4] = "Expression";
  token_types[-5] = "Echo";
  token_types[-6] = "String";
  token_types[-7] = "Statement";
  token_types[-8] = "If";
  token_types[-9] = "Else";
  token_types[-10] = "While";
  token_types[-11] = "Return";
  token_types[-12] = "Operator";
  token_types[-13] = "Unkown";

  for (int i = 0; i < tokenss.size(); i++) {
	  std::cout << "Type: " << token_types[tokenss.at(i).tok_type] << std::endl << "Value: " << tokenss.at(i).val << std::endl << "--------------------" << std::endl;
  }
}