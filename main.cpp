#include <algorithm>
#include <string>
#include <iostream>
#include <vector>
#include <array>
#include <assert.h>

using namespace std;

using ii = pair<int, int>;
array<string, 15> board;

struct Expr
{
    ii pos1, pos2;
    ii op;
    vector<ii> res;

    Expr() = default;
    Expr(ii pos1_, ii op_, ii pos2_, vector<ii> res_)
        : pos1(pos1_), pos2(pos2_), op(op_), res(res_) {}

    int calc(char a, char b)
    {
        int av = a - '0';
        int bv = b - '0';

        char opv = board[op.first][op.second];

        if (opv == '+')
            return av + bv;

        if (opv == 'x')
            return av * bv;

        if (opv == '-')
            return av - bv;

        assert(false);

        return -1;
    }

    bool possible_res(int v)
    {
        if (v < 0 || v > 81)
            return false;

        if (res.size() == 1)
        {
            if (v >= 10)
                return false;

            auto resv = board[res[0].first][res[0].second];

            if (resv == '.' || resv == v + '0')
                return true;
        }
        else
        {
            if (v < 10)
                return false;

            auto ten = board[res[0].first][res[0].second];
            auto one = board[res[1].first][res[1].second];

            if (ten == '.' && one == '.')
                return true;

            if (ten == '.' && one == (v % 10 + '0'))
                return true;

            if (one == '.' && ten == (v / 10 + '0'))
                return true;

            return (ten - '0') * 10 + one - '0' == v;
        }

        return false;
    }

    bool ok()
    {
        auto pos1v = board[pos1.first][pos1.second];
        auto pos2v = board[pos2.first][pos2.second];

        if (pos1v == '.' && pos2v == '.')
        {
            for (char d1 = '0'; d1 <= '9'; d1++)
                for (char d2 = '0'; d2 <= '9'; d2++)
                    if (possible_res(calc(d1, d2)))
                        return true;

            return false;
        }
        else if (pos1v == '.')
        {
            for (char d = '0'; d <= '9'; d++)
                if (possible_res(calc(d, pos2v)))
                    return true;

            return false;
        }
        else if (pos2v == '.')
        {
            for (char d = '0'; d <= '9'; d++)
                if (possible_res(calc(pos1v, d)))
                    return true;

            return false;
        }
        else
        {
            auto r = calc(pos1v, pos2v);

            return possible_res(r);
        }
    }
};

vector<int> relates[15][13];
vector<Expr> exprs;

string find_cand(int x, int y)
{
    string res;

    for (char d = '0'; d <= '9'; d++)
    {
        board[x][y] = d;

        if (all_of(relates[x][y].begin(), relates[x][y].end(), [](auto& r) { return exprs[r].ok(); }))
        {
            res.push_back(d);
        }
    }

    board[x][y] = '.';

    return res;
}

bool possible_check()
{
    for (int x = 0; x < 15; x++)
    {
        for (int y = 0; y < 13; y++)
        {
            if (board[x][y] != '.')
                continue;

            auto cand = find_cand(x, y);

            if (cand.empty())
                return false;
        }
    }

    return true;
}

bool fill_one()
{
    bool succeed = false;
    for (int x = 0; x < 15; x++)
    {
        for (int y = 0; y < 13; y++)
        {
            if (board[x][y] != '.')
                continue;

            auto cand = find_cand(x, y);

            if (cand.empty())
                return false;

            if (cand.size() > 1)
                continue;

            succeed = true;
            board[x][y] = cand[0];
        }
    }

    return succeed;
}

bool find_answer()
{
    if (!possible_check())
        return false;

    auto copied = board;

    while (fill_one());

    if (!possible_check())
    {
        board = copied;
        return false;
    }

    ii minCandPos{ -1,-1 };
    string minCand;

    for (int x = 0; x < 15; x++)
    {
        for (int y = 0; y < 13; y++)
        {
            if (board[x][y] != '.')
                continue;

            auto cand = find_cand(x, y);

            if (minCand.empty() || cand.size() < minCand.size())
            {
                minCandPos = { x,y };
                minCand = cand;
            }
        }
    }

    if (minCand.empty())
        return true;

    for (auto& c : minCand)
    {
        board[minCandPos.first][minCandPos.second] = c;

        if (find_answer())
            return true;
    }

    board = copied;
    return false;
}

void init_exprs()
{
    int hor_one[] = { 2, 11 };
    int hor_two[] = { 0, 5, 9, 14 };
    int ver_short[] = { 2, 10 };
    int ver_long[] = { 0, 4, 8, 12 };

    for (auto& h : hor_one)
    {
        relates[h][4].push_back(exprs.size());
        relates[h][6].push_back(exprs.size());
        relates[h][8].push_back(exprs.size());
        exprs.push_back(Expr({ h,4 }, { h,5 }, { h,6 }, { { h,8 } }));
    }

    for (auto& h : hor_two)
    {
        relates[h][0].push_back(exprs.size());
        relates[h][2].push_back(exprs.size());
        relates[h][4].push_back(exprs.size());
        exprs.push_back(Expr({ h,0 }, { h,1 }, { h,2 }, { { h,4 } }));

        relates[h][8].push_back(exprs.size());
        relates[h][10].push_back(exprs.size());
        relates[h][12].push_back(exprs.size());
        exprs.push_back(Expr({ h,8 }, { h,9 }, { h,10 }, { { h,12 } }));
    }

    for (auto& v : ver_short)
    {
        relates[5][v].push_back(exprs.size());
        relates[7][v].push_back(exprs.size());
        relates[9][v].push_back(exprs.size());
        exprs.push_back(Expr({ 5,v }, { 6,v }, { 7,v }, { { 9,v } }));
    }

    for (auto& v : ver_long)
    {
        relates[0][v].push_back(exprs.size());
        relates[2][v].push_back(exprs.size());
        relates[4][v].push_back(exprs.size());
        relates[5][v].push_back(exprs.size());
        exprs.push_back(Expr({ 0,v }, { 1,v }, { 2,v }, { { 4,v },{ 5,v } }));

        relates[9][v].push_back(exprs.size());
        relates[11][v].push_back(exprs.size());
        relates[13][v].push_back(exprs.size());
        relates[14][v].push_back(exprs.size());
        exprs.push_back(Expr({ 9,v }, { 10,v }, { 11,v }, { { 13,v },{ 14,v } }));
    }
}

int main()
{
    for (int i = 0; i < 15; i++)
        getline(cin, board[i]);

    init_exprs();

    if (!find_answer())
    {
        cout << "can't find answer" << endl;
        return 0;
    }

    for (int i = 0; i < 15; i++)
        cout << board[i] << endl;

    return 0;
}