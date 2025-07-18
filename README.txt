this is a toy interpreter I call "tinyLang"
the syntax is as below:

1. define variable_name(type : i(int)/c(char))
ex: define my_variable_integer(i)
note: dont forget the (i)/(c) after declaring a variable

2. eq variable_name,number_or_variable
ex: eq my_variable,26
ex2 : eq my_other_variable,my_variable

3. add dist_variable,source_NumberOrVariable,target_NumberOrVariable
ex: add my_variable,4,3
ex2 : add my_variable,my_other_variable,my_variable

4. mul dist_variable,source_NumberOrVariable,target_NumberOrVariable

5. dev dist_variable,source_NumberOrVariable,target_NumberOrVariable

6. mod dist_variable,source_NumberOrVariable,target_NumberOrVariable


NOTE: don't use spaces before or after commas