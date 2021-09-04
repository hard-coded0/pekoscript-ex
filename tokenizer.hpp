#pragma once
namespace Tokenizer {
  enum Tokens {
    tok_func       = -1,
    tok_identifier = -2,
    tok_number     = -3,
    tok_expression = -4,
    tok_echo       = -5,
    tok_string     = -6,
    tok_statement  = -7,
    tok_if         = -8,
    tok_else       = -9,
    tok_while      = -10,
    tok_return     = -11, 
    tok_op         = -12,
    tok_unkown     = -13,
  };

  enum StatementTypes {
    var_dec   = -1,
    func_dec  = -2,
    func_call = -3,
  };

  struct Token {
    int         tok_type;
    std::string val;
  };

  struct Statement {
    int                statement_type;
    std::vector<Token> tokens;
  };

  static bool isop(char poss_op) {
    if(
      poss_op == '*' ||
      poss_op == '/' ||
      poss_op == '%' ||
      poss_op == '+' ||
      poss_op == '-'
    ) {
      return true;
    } else {
      return false;
    }
  }

  // This splits each token into the different types
  static std::vector<Token> SplitToks(std::string code) {
    static std::string IdentifierStr;
    static double NumVal;

    // This will be the returned list
    std::vector<Token> token_list;

    for(int i = 0; i < code.length(); i++) {
      // Skips all whitespace
      while(isspace(code[i]) || code[i] == '\n' || code[i] == '\t') {
        i++;
      }

      // Checks for identifiers
      if(isalpha(code[i])) {
        IdentifierStr = code[i];

        i++;
        while(isalnum(code[i]) || code[i] == '_') {
          IdentifierStr += code[i];
          i++;
        }

        // Checks if the identifier is already reserved
        if(IdentifierStr == "func") {
          token_list.push_back(Token({tok_func, "func"}));
        } else if(IdentifierStr == "echo") {
          token_list.push_back(Token({tok_echo, "echo"}));
        } else if(IdentifierStr == "while") {
          token_list.push_back(Token({tok_while, "while"}));
        } else if(IdentifierStr == "if") {
          token_list.push_back(Token({tok_if, "if"}));
        } else if(IdentifierStr == "else") {
          token_list.push_back(Token({tok_else, "else"}));
        } else if(IdentifierStr == "return") {
          token_list.push_back(Token({tok_return, "return"}));
        } else {
          token_list.push_back(Token({tok_identifier, IdentifierStr}));
        }
        i--;
      
      // Checks for a number
      } else if (isdigit(code[i]) || code[i] == '.') {
        static std::string NumStr = "";

        do {
          NumStr += code[i];
          i++;
        } while(isdigit(code[i]) || code[i] == '.');

        token_list.push_back(Token({tok_number, NumStr}));
        
        i--;
        NumStr = "";
      // Handles expressions in parentheses
      } else if(isop(code[i])) {
        std::string op = "";
        op += code[i];
        token_list.push_back(Token({tok_op, op}));
      } else if (code[i] == '(') {
        static std::string Expr = "(";
        // How many parens until the expression is complete
        int parens_to_find = 1;
        i++;

        // Loops through until the expression is completely uncovered
        while(parens_to_find > 0 && i < code.length()) {
          if(code[i] == ')') {
            parens_to_find -= 1;
          }

          if(code[i] == '(') {
            parens_to_find += 1;
          }

          if(code[i] == '\n' || code[i] == '\t') {
            Expr += code[i];
            i++;
          }
        }

        token_list.push_back(Token({tok_expression, Expr}));

      // For handling a statement
      } else if (code[i] == '{') {
        static std::string Statement = "{";
        
        // How many braces until the Statement is fully uncovered
        int braces_to_find = 1;
        i++;

        while(braces_to_find > 0 && i < code.length()) {
          if(code[i] == '}') {
            braces_to_find -= 1;
          }

          if(code[i] == '{') {
            braces_to_find += 1;
          }

          Statement += code[i];
          i++;
        }

        token_list.push_back(Token({tok_statement, Statement}));

      // For handling arrays
      } else if (code[i] == '[') {
        static std::string Array = "[";

        // How many braces until the array is complete
        int braces_to_find = 1;
        i++;

        while(braces_to_find > 0 && i < code.length()) {
          if(code[i] == ']') {
            braces_to_find -= 1;
          }

          if(code[i] == '[') {
            braces_to_find += 1;
          }

          Array += code[i];
          i++;
        }

        token_list.push_back(Token({tok_expression, Array}));

      // For handling strings
      } else if (code[i] == '"') {
        static std::string String = "";
        i++;

        while(code[i] != '"' && i < code.length()) {
          String += code[i];
          i++;
        }

        token_list.push_back(Token({tok_string, String}));
        String = "";
      } else if (code[i] == '\'') {
        static std::string String = "";
        i++;

        while(code[i] != '\'' && i < code.length()) {
          String += code[i];
          i++;
        }

        token_list.push_back(Token({tok_string, String}));
        String = "";
      } else if(code[i] != '\n') {
        std::string unknown = "";
        unknown += code[i];
        token_list.push_back(Token({tok_unkown, unknown}));
      }
    }

    return token_list;
  }

  static int GetStatementType(std::vector<Tokenizer::Token> tokens) {
    for(int i = 0; i < tokens.size(); i++) {
      // If the token is an identifier
      if(tokens[i].tok_type == Tokenizer::tok_identifier) {
        while(tokens[i].tok_type != Tokenizer::tok_expression && tokens[i].val != "=") {
          i++;
        }

        // Then it checks if it is a variable declaration or function call
        if(tokens[i].val == "=") {
          return var_dec;
        } else {
          return func_call;
        }
      } else if (tokens[i].tok_type == Tokenizer::tok_func) {
        return func_dec;
      }
    }
  }

  // This makes separates each statement in the tokens out
  static std::vector<Statement> FinTokens(std::vector<Token> tokens) {
    std::vector<Statement> fin_state_list;

    for(int i = 0; i < tokens.size(); i++) {
      // This will store one statements worth of tokens
      std::vector<Token> inn_tok_list;

      // Adds the tokens to the statement
      
      while(tokens[i].val != ";" && i < tokens.size()-1) {
        inn_tok_list.push_back(tokens.at(i));
        i++;
      }

      inn_tok_list.push_back(tokens.at(i));
      i++;

      inn_tok_list.push_back(Token({tok_unkown, ";"}));

      // Adds the statement to the overall vector
      if(inn_tok_list.size() > 0) {
        fin_state_list.push_back(Statement({GetStatementType(inn_tok_list), inn_tok_list}));
      }
    }

    return fin_state_list;
  }
}