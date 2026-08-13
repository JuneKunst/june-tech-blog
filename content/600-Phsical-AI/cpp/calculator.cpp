#include <iostream>
#include <regex>
#include <string>

using namespace std;

int main() {
    // 정수와 소수, 양수와 음수를 모두 허용하는 숫자 정규식
    const string number = R"([-+]?(?:\d+(?:\.\d*)?|\.\d+))";
    const regex numberPattern("^\\s*" + number + "\\s*$");
    const regex operatorPattern("^\\s*[+\\-*/]\\s*$");

    string firstInput;
    string operatorInput;
    string secondInput;

    cout << "간단한 계산기입니다.\n";
    cout << "숫자, 연산자, 숫자 순서로 입력하세요.\n";

    cout << "첫 번째 숫자: ";
    getline(cin, firstInput);
    if (!regex_match(firstInput, numberPattern)) {
        cerr << "입력 오류: 첫 번째 값은 숫자여야 합니다.\n";
        return 1;
    }

    cout << "연산자 (+, -, *, /): ";
    getline(cin, operatorInput);
    if (!regex_match(operatorInput, operatorPattern)) {
        cerr << "입력 오류: 연산자는 +, -, *, / 중 하나여야 합니다.\n";
        return 1;
    }

    cout << "두 번째 숫자: ";
    getline(cin, secondInput);
    if (!regex_match(secondInput, numberPattern)) {
        cerr << "입력 오류: 두 번째 값은 숫자여야 합니다.\n";
        return 1;
    }

    try {
        // 동적 메모리 할당: 두 피연산자를 저장할 공간을 생성합니다.
        double* left = new double(stod(firstInput));
        double* right = new double(stod(secondInput));
        const char op = operatorInput[operatorInput.find_first_not_of(" \t")];
        double result = 0.0;

        if (op == '/' && *right == 0.0) {
            cerr << "계산 오류: 0으로 나눌 수 없습니다.\n";
            delete left;
            delete right;
            return 1;
        }

        switch (op) {
            case '+':
                result = *left + *right;
                break;
            case '-':
                result = *left - *right;
                break;
            case '*':
                result = *left * *right;
                break;
            case '/':
                result = *left / *right;
                break;
            default:
                // 정규식에서 이미 걸러지지만, 예외 상황을 한 번 더 처리합니다.
                cerr << "계산 오류: 지원하지 않는 연산자입니다.\n";
                delete left;
                delete right;
                return 1;
        }

        cout << "결과: " << *left << ' ' << op << ' ' << *right
             << " = " << result << '\n';

        // 동적 메모리 해제: 할당한 메모리는 사용 후 반드시 삭제합니다.
        delete left;
        delete right;
    } catch (const exception& error) {
        cerr << "입력 오류: 숫자를 읽을 수 없습니다. (" << error.what() << ")\n";
        return 1;
    }

    return 0;
}
