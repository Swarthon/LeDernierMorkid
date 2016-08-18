#include "UserInput.h"

void UserInput::run(){
	while(1){
		std::string command = in();
		std::string answer = process(command);
		out(answer);
	}
}
std::string UserInput::in(){
	std::string s;
	getline(std::cin,s);

	return s;
}
void UserInput::out(std::string s){
	std::cout << s << std::endl;
}