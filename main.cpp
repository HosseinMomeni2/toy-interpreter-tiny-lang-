#include <iostream>
#include <utility>
#include <vector>
#include <fstream>

class Variable
{
    std::string name;
    char type;
    int val_int;
    char val_chr;

    friend class Command;
};

const std::vector<std::string> valid_words = {"LABEL", "define", "read", "print", "add", "sub", "mul", "div", "mod", "eq"};
std::vector<std::pair<std::string, int>> labels;
std::vector<Variable> variables;

class Command
{
    std::string line;
    int line_number;
    int isValid;
    std::string main_word;
    int main_word_code;
    char params_type[3];
    int params_int[3];
    char params_char[3];
    std::string label_name;
    char define_type;

public:
    explicit Command(std::string L, int number)
    {
        line_number = number;
        if(L.size() > 100)
        {
            isValid = -100; ///too long line
            return;
        }

        L.push_back(',');


        line = std::move(L);
        int validation_result = check_for_syntax_error();
        if(validation_result < 0){
            isValid = validation_result;
            return;
        }

        isValid = 1;
        main_word = valid_words[validation_result];
        main_word_code = validation_result;

        if(main_word_code == 0) //Label or define
        {
            set_label_name();
        }
        else if(main_word_code == 1) {
            set_label_name();
            set_define_type();
        }
        else {
            set_params();
        }

    }

    int check_for_syntax_error()
    {
        std::string word;
        for(auto itr : line){
            if(itr == ' ') break;
            word.push_back(itr);
            if(word.size() > 10) return -1; ///invalid word
        }

        for(int i=0; i<valid_words.size(); i++)
            if(valid_words[i] == word) return i;

        return -2; ///unknown word
    }

    int set_params()
    {
        int param_index = 0;
        int i;
        std::string tmp_param;
        for(i = (int)main_word.size() + 1; i<line.size(); i++)
        {
            if(line[i] == ' ') return -3; ///syntax error

            if(line[i] == ',')
            {
                if(tmp_param.empty()) return -3; ///syntax error

                else if(isdigit(tmp_param[0])) // if it is a number
                {
                    for(auto x : tmp_param)
                    {
                        if(!isdigit(x))
                            return -4; ///invalid variable name
                    }

                    int tmp_param_int = tmp_param[0] - '0';
                    for(int ind=1; ind<tmp_param.size(); ind++)
                    {
                        tmp_param_int *= 10;
                        tmp_param_int += tmp_param[i] - '0';
                    }

                    params_type[param_index] = 'i';
                    params_int[param_index++] = tmp_param_int;
                }

                else if(tmp_param[0] == '\'') //if it is a char
                {
                    if(tmp_param.size() != 3) return -4; ///invalid variable name

                    if(tmp_param[2] != '\'') return -4;  ///invalid variable name

                    params_type[param_index] = 'c';
                    params_char[param_index++] = tmp_param[1];
                }

                else //it is a variable
                {
                    int variable_index = -1;
                    for(int ind=0; ind<variables.size(); ind++)
                    {
                        if(tmp_param == variables[ind].name)
                        {
                            variable_index = ind;
                            break;
                        }
                    }
                    if(variable_index == -1) return -5; ///variable is not initialized

                    params_type[param_index] = 'v';
                    params_int[param_index++] = variable_index;
                }

                tmp_param.clear();
                if(param_index == 3) return 1;
            }
            else
            {
                tmp_param.push_back(line[i]);
            }
        }

        return 1;
    }

    void set_label_name()
    {
        std::string l_name;
        for(int i=(int)main_word.size()+1; i<line.size()-1; i++)
            l_name.push_back(line[i]);

        label_name = l_name;
    }

    void set_define_type()
    {
        label_name.pop_back();
        define_type = label_name.back();
        label_name.pop_back();
        label_name.pop_back();
    }

    bool is_a_label()
    {
        if(main_word_code == 0) return true;
        return false;
    }

    int exec()
    {
        switch (main_word_code)
        {
            case 0: ///LABEL
            {
                int index = -1;
                for (const auto& x: labels) {
                    if (x.first == label_name) {
                        index = 1;
                        break;
                    }
                }
                if (index < 0) {
                    labels.emplace_back(label_name, line_number);
                }
                break;
            }

            case 1: ///define
            {
                bool exist = false;
                for(auto &x : variables)
                    if(x.name == label_name)
                    {
                        exist = true;
                        break;
                    }

                if(exist) return -12; ///variabla exists

                Variable v;
                v.name = label_name;
                v.type = define_type;
                v.val_chr = '\0';
                v.val_int = 0;

                variables.push_back(v);

                break;
            }

            case 2: ///read
            {
                if (params_type[0] != 'v') return -11; // read error

                if(variables[params_int[0]].type == 'i') std::cin >> variables[params_int[0]].val_int;
                else if(variables[params_int[0]].type == 'c') std::cin >> variables[params_int[0]].val_chr;

                break;
            }
            case 3: ///print
            {
                if (params_type[0] != 'v') return -12; // print error

                if(variables[params_int[0]].type == 'i') std::cout << variables[params_int[0]].val_int << std::endl;
                else if(variables[params_int[0]].type == 'c') std::cout << variables[params_int[0]].val_chr << std::endl;

                break;
            }

            case 4: ///add
            {
                if(params_type[0] != 'v') return -15; ///equation error

                char operation_type = params_type[1];
                if(operation_type == 'v')
                {
                    operation_type = variables[params_int[1]].type;
                }


                if(operation_type == 'i')
                {
                    int operand1=1, operand2=1;

                    if(params_type[1] == 'v') operand1 = variables[params_int[1]].val_int;
                    else if(params_type[1] == 'i') operand1 = params_int[1];
                    else if(params_type[1] == 'c') operand1 = int((unsigned char)params_char[1]);

                    if(params_type[2] == 'v') operand2 = variables[params_int[2]].val_int;
                    else if(params_type[2] == 'i') operand2 = params_int[2];
                    else if(params_type[2] == 'c') operand2 = int((unsigned char)params_char[2]);

                    variables[params_int[0]].val_int = operand1 + operand2;
                }
                else if(operation_type == 'c')
                {
                    char operand1=1, operand2=1;

                    if(params_type[1] == 'v') operand1 = variables[params_int[1]].val_chr;
                    else if(params_type[1] == 'i') operand1 = (char)params_int[1];
                    else if(params_type[1] == 'c') operand1 = params_char[1];

                    if(params_type[2] == 'v') operand2 = variables[params_int[2]].val_chr;
                    else if(params_type[2] == 'i') operand2 = (char)params_int[2];
                    else if(params_type[2] == 'c') operand2 = params_char[2];

                    variables[params_int[0]].val_chr = operand1 + operand2;
                }


                break;
            }
            case 5: ///sub
            {
                if(params_type[0] != 'v') return -15; ///equation error

                char operation_type = params_type[1];
                if(operation_type == 'v')
                {
                    operation_type = variables[params_int[1]].type;
                }


                if(operation_type == 'i')
                {
                    int operand1=1, operand2=1;

                    if(params_type[1] == 'v') operand1 = variables[params_int[1]].val_int;
                    else if(params_type[1] == 'i') operand1 = params_int[1];
                    else if(params_type[1] == 'c') operand1 = int((unsigned char)params_char[1]);

                    if(params_type[2] == 'v') operand2 = variables[params_int[2]].val_int;
                    else if(params_type[2] == 'i') operand2 = params_int[2];
                    else if(params_type[2] == 'c') operand2 = int((unsigned char)params_char[2]);

                    variables[params_int[0]].val_int = operand1 - operand2;
                }
                else if(operation_type == 'c')
                {
                    char operand1=1, operand2=1;

                    if(params_type[1] == 'v') operand1 = variables[params_int[1]].val_chr;
                    else if(params_type[1] == 'i') operand1 = (char)params_int[1];
                    else if(params_type[1] == 'c') operand1 = params_char[1];

                    if(params_type[2] == 'v') operand2 = variables[params_int[2]].val_chr;
                    else if(params_type[2] == 'i') operand2 = (char)params_int[2];
                    else if(params_type[2] == 'c') operand2 = params_char[2];

                    variables[params_int[0]].val_chr = operand1 - operand2;
                }
                break;
            }
            case 6: ///mul
            {
                if(params_type[0] != 'v') return -15; ///equation error

                char operation_type = params_type[1];
                if(operation_type == 'v')
                {
                    operation_type = variables[params_int[1]].type;
                }


                if(operation_type == 'i')
                {
                    int operand1=1, operand2=1;

                    if(params_type[1] == 'v') operand1 = variables[params_int[1]].val_int;
                    else if(params_type[1] == 'i') operand1 = params_int[1];
                    else if(params_type[1] == 'c') operand1 = int((unsigned char)params_char[1]);

                    if(params_type[2] == 'v') operand2 = variables[params_int[2]].val_int;
                    else if(params_type[2] == 'i') operand2 = params_int[2];
                    else if(params_type[2] == 'c') operand2 = int((unsigned char)params_char[2]);

                    variables[params_int[0]].val_int = operand1 * operand2;
                }
                else if(operation_type == 'c')
                {
                    char operand1=1, operand2=1;

                    if(params_type[1] == 'v') operand1 = variables[params_int[1]].val_chr;
                    else if(params_type[1] == 'i') operand1 = (char)params_int[1];
                    else if(params_type[1] == 'c') operand1 = params_char[1];

                    if(params_type[2] == 'v') operand2 = variables[params_int[2]].val_chr;
                    else if(params_type[2] == 'i') operand2 = (char)params_int[2];
                    else if(params_type[2] == 'c') operand2 = params_char[2];

                    variables[params_int[0]].val_chr = operand1 * operand2;
                }
                break;
            }
            case 7: ///div
            {
                if(params_type[0] != 'v') return -15; ///equation error

                char operation_type = params_type[1];
                if(operation_type == 'v')
                {
                    operation_type = variables[params_int[1]].type;
                }


                if(operation_type == 'i')
                {
                    int operand1=1, operand2=1;

                    if(params_type[1] == 'v') operand1 = variables[params_int[1]].val_int;
                    else if(params_type[1] == 'i') operand1 = params_int[1];
                    else if(params_type[1] == 'c') operand1 = int((unsigned char)params_char[1]);

                    if(params_type[2] == 'v') operand2 = variables[params_int[2]].val_int;
                    else if(params_type[2] == 'i') operand2 = params_int[2];
                    else if(params_type[2] == 'c') operand2 = int((unsigned char)params_char[2]);

                    variables[params_int[0]].val_int = operand1 / operand2;
                }
                else if(operation_type == 'c')
                {
                    char operand1=1, operand2=1;

                    if(params_type[1] == 'v') operand1 = variables[params_int[1]].val_chr;
                    else if(params_type[1] == 'i') operand1 = (char)params_int[1];
                    else if(params_type[1] == 'c') operand1 = params_char[1];

                    if(params_type[2] == 'v') operand2 = variables[params_int[2]].val_chr;
                    else if(params_type[2] == 'i') operand2 = (char)params_int[2];
                    else if(params_type[2] == 'c') operand2 = params_char[2];

                    variables[params_int[0]].val_chr = operand1 / operand2;
                }
                break;
            }
            case 8: ///mod
            {
                if(params_type[0] != 'v') return -15; ///equation error

                char operation_type = params_type[1];
                if(operation_type == 'v')
                {
                    operation_type = variables[params_int[1]].type;
                }


                if(operation_type == 'i')
                {
                    int operand1=1, operand2=1;

                    if(params_type[1] == 'v') operand1 = variables[params_int[1]].val_int;
                    else if(params_type[1] == 'i') operand1 = params_int[1];
                    else if(params_type[1] == 'c') operand1 = int((unsigned char)params_char[1]);

                    if(params_type[2] == 'v') operand2 = variables[params_int[2]].val_int;
                    else if(params_type[2] == 'i') operand2 = params_int[2];
                    else if(params_type[2] == 'c') operand2 = int((unsigned char)params_char[2]);

                    variables[params_int[0]].val_int = operand1 % operand2;
                }
                else if(operation_type == 'c')
                {
                    char operand1=1, operand2=1;

                    if(params_type[1] == 'v') operand1 = variables[params_int[1]].val_chr;
                    else if(params_type[1] == 'i') operand1 = (char)params_int[1];
                    else if(params_type[1] == 'c') operand1 = params_char[1];

                    if(params_type[2] == 'v') operand2 = variables[params_int[2]].val_chr;
                    else if(params_type[2] == 'i') operand2 = (char)params_int[2];
                    else if(params_type[2] == 'c') operand2 = params_char[2];

                    variables[params_int[0]].val_chr = operand1 % operand2;
                }
                break;
            }
            case 9: ///eq
            {
                if(params_type[0] != 'v') return -15;

                if(variables[params_int[0]].type == 'i')
                {
                    if(params_type[1] == 'i')
                    {
                        variables[params_int[0]].val_int = params_int[1];
                    }
                    else if(params_type[1] == 'c')
                    {
                        variables[params_int[0]].val_int = int((unsigned char)params_char[1]);
                    }
                    else return -15;
                }
                else if(variables[params_int[0]].type == 'c')
                {
                    if(params_type[1] == 'i')
                    {
                        variables[params_int[0]].val_chr = (char)params_int[1];
                    }
                    else if(params_type[1] == 'c')
                    {
                        variables[params_int[0]].val_chr = params_char[1];
                    }
                    else return -15;
                }

                break;
            }
        }

        return 1;
    }


};

int main() {
    std::string file_address = "D:\\proj\\tinyLang\\1.txt";

    std::ifstream tLang_file(file_address);

    std::string one_line;
    int line_number = 1;
    while(std::getline(tLang_file, one_line))
    {
        Command c(one_line, line_number);
        if(c.is_a_label())
        {
            std::cout << "A label" << std::endl;
        }
        else
        {
            int run = c.exec();

            if(run < 0)
            {
                std::cout << "error " << run << " happened in line "  << line_number << std::endl;
                break;
            }
        }

        line_number ++;
    }

    return 0;
}
