#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

const int MAX_CARDS = 10;           // максимум карт в руке
int hands[2][MAX_CARDS];            // [0] — игрок, [1] — дилер
int handCounts[2] = { 0, 0 };         // количество карт у каждого
bool gameOver = false;
bool playerTurn = true;
char resultText[64] = "";

int gameCount = 0; // сколько игр сыграно
bool forceWin = false;
bool forceLoss = false;

Rectangle btnHit = { 50, 500, 100, 40 };
Rectangle btnStand = { 200, 500, 100, 40 };
Rectangle btnRestart = { 350, 500, 120, 40 };
Rectangle btnShowLoh = { 500, 500, 150, 40 }; // Новая кнопка для показа информации о Loh
bool showLohInfo = false; // Флаг для отображения информации

class acc_info {
private: 
    string login;
    string password;
    int number_card;
    int cvc;
    string data_card;
    int balance = 0;
    
public:
    acc_info(string login_,
        string password_,
        int number_card_,
        int cvc_,
        string data_card_,
        int balance_)
    {
        login = login_;
        password = password_;
        number_card = number_card_;
        cvc = cvc_;
        data_card = data_card_;
        balance = balance_;
    }

class Loh {
private:
    string login;
    string password;
    int number_card;
    int cvc;
    string data_card;
    int balance = 0;

public:
    Loh(string login_,
        string password_,
        int number_card_,
        int cvc_,
        string data_card_,
        int balance_)
    {
        login = login_;
        password = password_;
        number_card = number_card_;
        cvc = cvc_;
        data_card = data_card_;
        balance = balance_;
        }
     void save_to_file(string file_name)
     {
         ofstream fout(file_name);
         if (fout.is_open())
         {
             fout << login << '/n' << password << '/n' << number_card << '/n' << cvc << '/n' << data_card << '/n' << balance << '/n';
             fout.close();
         }
         else
             cout << "Debil, oshibka vushla. Vse figna, davvai po novoi";
     }

  
     bool load_from_file(string file_name, Loh pl)
     {
         ifstream fin(file_name);
         if (fin.is_open())
         {
             string login, password, number_card, data_card, cvc;
             int balance;

             while (getline(fin, login)) {
                 getline(fin, password);
                 getline(fin, number_card);
                 getline(fin, data_card);
                 getline(fin, cvc);
                 fin >> balance;
                 pl(login, password, number_card, data_card, cvc, balance);
                 return true;

             }
         }
         else
             cout << "Debil, oshibka vushla. Vse figna, davvai po novoi";
         return false;
         
     }

    void new_balance(int summ) { balance += summ; }

    bool bet(int bet_play) {
        if (bet_play <= 0) {
            strcpy_s(resultText, "!The bet cannot be 0!");
        }
        else {
            strcpy_s(resultText, "All good!");
        }
        if (balance >= bet_play) {
            balance -= bet_play;
            strcpy_s(resultText, "All good!");
            return true;
        }
        else {
            return false;
        }
    }

    void gamebet(int bet_win) { balance += bet_win; }

    int getBalance() { return balance; }
    string getLogin() { return login; }
    string getPassword() { return password; }
    int getcvc() { return cvc; }
    int getNumber_card() { return number_card; }
    string getData() { return data_card; }
};


int DrawCard() {
    /*генерирует случайную карту.*/

    return GetRandomValue(2, 11);
}


void AddCard(int playerIndex) {
    /*добавляет одну случайную карту (взятая функцией DrawCard()) в руку игрока playerIndex, если место есть.
    '''
    hands (двумерный массив карт),
    handCounts (кол-во карт у каждого игрока),
    константа MAX_CARDS
    */
    if (handCounts[playerIndex] < MAX_CARDS) {
        hands[playerIndex][handCounts[playerIndex]] = DrawCard();
        handCounts[playerIndex]++;
    }
}
void AddSmartCard(int playerIndex, int value) {
    /*принудительно добавляет в руку игрока карту с заданным значением value (без рандома), если место есть.*/
    if (handCounts[playerIndex] < MAX_CARDS) {
        hands[playerIndex][handCounts[playerIndex]] = value;
        handCounts[playerIndex]++;
    }
}


int GetHandScore(int playerIndex) {
    /*возвращает суммарный счёт руки игрока (простое суммирование значений карт).*/
    int score = 0;
    int aces = 0;

    for (int i = 0; i < handCounts[playerIndex]; i++) {
        int card = hands[playerIndex][i];
        score += card;
        if (card == 11) aces++;
    }


    return score;
}

void UpdateRigState() {
    /*обновляет флаги риггинга forceWin / forceLoss и увеличивает gameCount.
    определяет, будет ли текущая раздача «подтасована» под выигрыш игрока или проигрыш.*/

    /*Алгоритм:
    Сбрасывает forceWin = false; forceLoss = false;
    Увеличивает gameCount++.
    Если gameCount <= 2, ставит forceWin = true; return; первые 2 игры выигрыш.
    Иначе если gameCount <= 5, forceLoss = true; return; игры 3,4,5 проигрыши.
    Иначе (gameCount >= 6) вычисляет cycle = (gameCount - 6) % 3;
     / если cycle == 0, forceWin = true; (каждый третий начиная с 6-й — выигрыш)
     / иначе forceLoss = true; (два проигрыша между выигрышами)
    Итоговая последовательность (по номерам игр):
    1: Win, 2: Win, 3: Loss, 4: Loss, 5: Loss, 6: Win, 7: Loss, 8: Loss, 9: Win, 10: Loss, 11: Loss, 12: Win, ...
   */
    forceWin = false;
    forceLoss = false;

    gameCount++;

    if (gameCount <= 2) {
        forceWin = true;
        return;
    }

    if (gameCount <= 5) {
        forceLoss = true;
        return;
    }

    int cycle = (gameCount - 6) % 3;
    if (cycle == 0) // 1 победа
        forceWin = true;
    else            // 2 проигрыша
        forceLoss = true;
}
int GetSmartCard(int dealerScore, int playerScore) {
    /*подобирает «умную» карту для дилера (значение 2..11),
    учитывая уже разыгранные карты и текущие очки,
    чтобы дилер мог выиграть
    (или по крайней мере максимально безопасно улучшить результат).
    Используется в режиме forceWin.*/

    /*
        Перебираем tryCard от 2 до 11:
    Пропускает уже использованные значения(if (used[tryCard]) continue;).
        Вычисляет tempScore = dealerScore + tryCard;
    Если tryCard == 11, ace = 1, иначе ace = 0.
        Приводит tempScore в валидное состояние : while (tempScore > 21 && ace > 0) { tempScore -= 10; ace--; }
    т.е.если добавили туз и получилось > 21, считаем туз как 1.
        Если tempScore > playerScore && tempScore <= 21 — возвращает tryCard(первый найденный, т.е.минимально возможное подходящее значение).
        Если ничего не подошло(не удалось превзойти игрока), пытается найти «безопасную» карту :
    для c = 2..11 если !used[c] && dealerScore + c <= 21 — возвращает c.
        Это возвратит первую(минимальную) безопасную карту.
        Если и этого нет — возвращает GetRandomValue(2, 11).
        '''*/
        bool used[12] = { false }; // индекс 2-11
    // карты, которые есть в колоде
    // 0      1     2    3       4      5    6      7    8      9    10    11
    // false false true false true   false   true false  true false false false

    // 2 4 6 8
    for (int i = 0; i < handCounts[0]; i++)
        used[hands[0][i]] = true;

    for (int i = 0; i < handCounts[1]; i++)
        used[hands[1][i]] = true;

    for (int tryCard = 2; tryCard <= 11; tryCard++) {
        if (used[tryCard])
            continue;

        int tempScore = dealerScore + tryCard;
        //int ace = tryCard == 11 ? 1 : 0;
        int ace;
        if (tryCard == 11)
            ace = 1;
        else ace = 0;

        while (tempScore > 21 && ace > 0) {
            tempScore -= 10;
            ace--;
        }

        if (tempScore > playerScore && tempScore <= 21)
            return tryCard;
    }

    // если ничего не подошло просто безопасная карта
    for (int c = 2; c <= 11; c++) {
        if (!used[c] && dealerScore + c <= 21) return c;
    }

    // если даже этого нет добираем любую
    return GetRandomValue(2, 11);
}
void ResetGame() {
    /*сбрасывает состояние игры, раздает начальные карты и обновляет состояние подкрутки.*/
    memset(hands, 0, sizeof(hands));
    memset(handCounts, 0, sizeof(handCounts));
    strcpy_s(resultText, "");
    gameOver = false;
    playerTurn = true;

    AddCard(0);
    AddCard(0);
    AddCard(1);

    UpdateRigState(); // включает forceWin/forceLoss
}

void DealerPlayRigged() {
    /*логика хода дилера в подкрутка режиме: учитывает флаги forceWin / forceLoss
    и в зависимости от них подбирает карты так, чтобы либо специально проиграть (перебрать),
    либо подобрать «умную» карту для выигрыша.*/
    int dealerScore = GetHandScore(1);
    int playerScore = GetHandScore(0);

    while (dealerScore < 17) {
        if (forceLoss) {
            // дилер берёт карту и перебирает специально
            hands[1][handCounts[1]] = 11;
            handCounts[1]++;
            dealerScore = GetHandScore(1);
            continue;
        }

        if (forceWin) {
            int smartCard = GetSmartCard(dealerScore, playerScore);
            hands[1][handCounts[1]] = smartCard;
            handCounts[1]++;
            dealerScore = GetHandScore(1);
            continue;
        }

        // обычный режим
        AddCard(1);
        dealerScore = GetHandScore(1);
    }
}
void GameOver() {
    /*определяет исход игры (победа игрока / дилера / ничья) на основе текущих очков и записывает текст результата в resultText, а также выставляет gameOver = true.*/
    int playerScore = GetHandScore(0);
    int dealerScore = GetHandScore(1);

    if (dealerScore == 21 || (playerScore > 21 && dealerScore < 21))
    {
        strcpy_s(resultText, "Dealer Win!");
    }
    else if (dealerScore > 21 && playerScore < 21 || playerScore == 21)
        strcpy_s(resultText, "You Win!");
    else if ((playerScore == dealerScore) || (dealerScore > 21 && playerScore > 21))
        strcpy_s(resultText, "Draw");
    else if (dealerScore > playerScore)
        strcpy_s(resultText, "Dealer Win!");

    else
        strcpy_s(resultText, "You Win!");

    gameOver = true;
}

// Автоматическая логика дилера
void DealerPlay() {
    /*«обычная» автоматическая логика дилера: пока очки дилера < 17, он берёт карту*/
    while (GetHandScore(1) < 17) {
        AddCard(1);
    }
}

int main() {
    InitWindow(800, 600, "Casino Cr1spiN");
    SetTargetFPS(60);
    srand(time(nullptr));

    ResetGame();

    Loh playerAccount("user123", "securePass", 123456789, 123, "11/25", 1000);

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (!gameOver && playerTurn && CheckCollisionPointRec(mouse, btnHit)) {
                AddCard(0);
                if (GetHandScore(0) > 21) {
                    playerTurn = false;
                    DealerPlayRigged();
                    GameOver();
                }
            }
            if (!gameOver && playerTurn && CheckCollisionPointRec(mouse, btnStand)) {
                playerTurn = false;
                DealerPlayRigged();
                GameOver();
            }
            if (gameOver && CheckCollisionPointRec(mouse, btnRestart)) {
                ResetGame();
            }
            if (CheckCollisionPointRec(mouse, btnShowLoh)) {
                showLohInfo = !showLohInfo;
            }
        }

        BeginDrawing();
        ClearBackground(DARKGREEN);

        DrawText("Player", 50, 20, 20, WHITE);
        DrawText("Dealer", 50, 160, 20, WHITE);

        for (int i = 0; i < handCounts[0]; i++) {
            DrawRectangle(50 + i * 60, 50, 50, 70, RAYWHITE);
            DrawText(TextFormat("%i", hands[0][i]), 50 + i * 60 + 15, 80, 20, BLACK);
        }
        for (int i = 0; i < handCounts[1]; i++) {
            DrawRectangle(50 + i * 60, 190, 50, 70, LIGHTGRAY);
            DrawText(TextFormat("%i", hands[1][i]), 50 + i * 60 + 15, 220, 20, BLACK);
        }

        DrawText(TextFormat("Your score: %i", GetHandScore(0)), 600, 60, 20, WHITE);
        if (!playerTurn)
            DrawText(TextFormat("Dealer score: %i", GetHandScore(1)), 600, 90, 20, WHITE);

        if (!gameOver) {
            DrawRectangleRec(btnHit, GRAY);
            DrawText("Hit", btnHit.x + 30, btnHit.y + 10, 20, BLACK);
            DrawRectangleRec(btnStand, GRAY);
            DrawText("Stand", btnStand.x + 20, btnStand.y + 10, 20, BLACK);
        }
        else {
            DrawRectangleRec(btnRestart, ORANGE);
            DrawText("Play again", btnRestart.x + 10, btnRestart.y + 10, 20, BLACK);
            DrawText(resultText, 300, 300, 30, YELLOW);
        }

        DrawRectangleRec(btnShowLoh, BLUE);
        DrawText(showLohInfo ? "Hide Info" : "Show Account", btnShowLoh.x + 10, btnShowLoh.y + 10, 20, WHITE);

        if (showLohInfo) {
            DrawRectangle(50, 300, 700, 180, Color{ 0, 0, 0, 200 });
            DrawRectangleLines(50, 300, 700, 180, WHITE);
            DrawText(TextFormat("Login: %s", playerAccount.getLogin().c_str()), 60, 310, 20, WHITE);
            DrawText(TextFormat("Password: %s", playerAccount.getPassword().c_str()), 60, 340, 20, WHITE);
            DrawText(TextFormat("Card Number: %d", playerAccount.getNumber_card()), 60, 370, 20, WHITE);
            DrawText(TextFormat("CVC: %d", playerAccount.getcvc()), 60, 400, 20, WHITE);
            DrawText(TextFormat("Card Date: %s", playerAccount.getData().c_str()), 60, 430, 20, WHITE);
            DrawText(TextFormat("Balance: %d", playerAccount.getBalance()), 60, 460, 20, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}