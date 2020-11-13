
#include"parser.h"

#include<stdio.h>
#include<iostream>
#include<algorithm>
#include<vector>
#include<time.h>
#include<queue>
#include<set>
#include<string>
#include<sstream>
#include<fstream>
using namespace std;

struct SatSol
{
    vector<vector<int>> C;
    int n, m, rem, level;
    vector<int> X, STAT;//0: unassigned, 1: decided, 2: implied
    vector<vector<set<vector<int>>>> POS;
    vector<vector<vector<int>>> LOG;
    vector<vector<int>> D, I;
    vector<int> ORDER;
    clock_t begt, endt, t;
    vector<int> PRE;
    vector<vector<int>> UNIT, FIXED;
    vector<int> LEVEL, aLOG;
    vector<vector<int>> WAT;
    int alevel;

    void showX()
    {
        for (int i = 0; i <= level; i++)
        {
            printf("level=%d: ", i);
            printf("%d ", D[i][0] * (i == 0 ? 1 : X[D[i][0]] ? 1 : -1));
            for (int j : I[i])
                printf("%d ", j*(X[j] ? 1 : -1));
            printf("\n");
        }
    }

    void showC(int c)
    {
        printf("clause %d: ", c);
        for (int y : C[c])
            printf("%d ", y);
        printf("\n");
        printf("p=%d q=%d\n", WAT[c][0], WAT[c][1]);
    }

    int assign(int x, int val, int stat)//assumption: x is unassigned
    {
        int res = -1;
        alevel++;
        LOG.push_back({});
        aLOG.push_back(x);
        rem--, X[x] = val, STAT[x] = stat, LEVEL[x] = level;
        for (vector<int> v : POS[x][val ^ 1])
        {
            int c = v[0];
            int &p = WAT[c][v[1]], &q = WAT[c][v[1] ^ 1], pt = p;
            //printf("x=%d val=%d stat=%d v0=%d v1=%d p=%d q=%d\n", x, val, stat, v[0], v[1], WAT[v[0]][0], WAT[v[0]][1]);
            for (p = max(p, q); p < C[c].size(); p++)
            {
                int y = C[c][p];
                if (p != q && !(STAT[abs(y)] > 0 && X[abs(y)] != y > 0))
                    break;
            }
            LOG.back().push_back({ c, v[1], pt, p });
            if (p < C[c].size())
            {
                int y = C[c][p];
                POS[abs(y)][y > 0].insert({ c, v[1] });
            }
            if (q >= C[c].size())
            {
                res = c;
                continue;
            }
            if (p >= C[c].size() && STAT[abs(C[c][q])] == 0)
            {
                UNIT.push_back({ C[c][q], c });
            }
        }
        POS[x][val ^ 1].clear();
        //printf("assign %d %d %d, unitsize=%d res=%d\n", x, val, stat, UNIT.size(), res);
        return res;
    }

    void unassign(int x)//assumption: x is assigned
    {
        rem++, STAT[x] = 0;
        for (vector<int> &v : LOG.back())
        {
            int c = v[0];
            int &p = WAT[c][v[1]];
            if (p < C[c].size())
            {
                int y = C[c][p];
                POS[abs(y)][y > 0].erase({ c, v[1] });
            }
            p = v[2];
            int y = C[c][p];
            POS[abs(y)][y > 0].insert({ c, v[1] });
        }
        alevel--;
        LOG.pop_back();
        aLOG.pop_back();
    }

    void check()
    {
        for (int i = 1; i <= n; i++)
            printf("%d ", X[i] ? 1 : 0);
        printf("\n");
        bool sat = true;
        for (int i = 0; i < m; i++)
        {
            bool valid = false;
            for (int x : C[i])
                valid |= X[abs(x)] != x < 0;
            if (!valid)
            {
                sat = false;
                printf("!!!not valid!!! i=%d\n", i);
                for (int x : C[i])
                    printf("%d %d\n", x, X[abs(x)]);
            }
        }
        if (sat)
            printf("~~~~~~~~~~~~~~   valid   ~~~~~~~~~~~~~\n");
    }

    void init(vector<vector<int>> &Ct, int nt)
    {
        C = Ct;
        n = nt;
        m = (int)C.size();
        rem = n;
        X.resize(n + 1);
        STAT.assign(n + 1, 0);
        POS.assign(n + 1, { {},{} });
        WAT.assign(m, { 0, 1 });
        for (int i = 0; i < m; i++)
            for (int j = 0; j < 2; j++)
            {
                int p = WAT[i][j];
                if (p < C[i].size())
                {
                    int x = C[i][p];
                    POS[abs(x)][x > 0].insert({ i, j });
                }
            }
        for (int i = 0; i < m; i++)
            if (C[i].size() == 1)
                UNIT.push_back({ C[i][0] , i });
        vector<vector<int>> cnt;
        for (int i = 1; i <= n; i++)
            cnt.push_back({ 0, i });
        for (vector<int> &c : C)
            for (int y : c)
                cnt[abs(y) - 1][0]++;
        sort(cnt.rbegin(), cnt.rend());
        ORDER.clear();
        for (int i = 0; i < n; i++)
            ORDER.push_back(cnt[i][1]);
        PRE.resize(n + 1);
        LEVEL.resize(n + 1);
        alevel = -1;
        level = 0;
        D.push_back({ -1, -1, -1 });
        I.push_back({});
    }

    string sol(double slice, double changetime)
    {
        t = clock();
        stringstream ss;
        for (; ; )
        {
            int err = -1, sat = 1;
            while (!UNIT.empty())
            {
                int x = UNIT.back()[0], val = x > 0, pre = UNIT.back()[1];
                UNIT.pop_back();
                if (STAT[abs(x)] != 0)
                {
                    if (X[abs(x)] == val)
                        continue;
                    else
                    {
                        printf("\n\nSTAT[abs(x)] != 0 && X[abs(x)] != val\n");
                        printf("x=%d val=%d pre=%d\n\n", x, val, pre);
                        for (vector<int> &c : C)
                            if (c.size() <= 1)
                            {
                                for (int y : c)
                                    printf("%d ", y);
                                printf("\n");
                            }
                        sat = 0;
                        break;
                    }
                }
                I.back().push_back(abs(x));
                PRE[abs(x)] = pre;
                err = assign(abs(x), val, 2);
                if (err > -1)
                {
                    set<int> cut(C[err].begin(), C[err].end());
                    set<int> tores;
                    for (int y : cut)
                        if (LEVEL[abs(y)] == level)
                            tores.insert(abs(y));
                    for (auto it = I.back().rbegin(); it != I.back().rend() && tores.size() > 1; it++)
                    {
                        if (cut.find(*it) != cut.end() || cut.find(-*it) != cut.end())
                        {
                            cut.insert(C[PRE[abs(*it)]].begin(), C[PRE[abs(*it)]].end());
                            cut.erase(*it);
                            cut.erase(-*it);
                            for (int y : C[PRE[abs(*it)]])
                                if (LEVEL[abs(y)] == level)
                                    tores.insert(abs(y));
                            tores.erase(abs(*it));
                        }
                    }
                    
                    if (cut.size() > max(5.0, n*(1 - slice)))
                    {
                        //printf("skip clause\n");
                        for (; D.size() > 1 && D.back()[2] == 1; D.pop_back(), I.pop_back(), level--)
                        {
                            for (; I.back().size() > 0; I.back().pop_back())
                                unassign(I.back().back());
                            unassign(D.back()[0]);
                        }
                        for (; I.back().size() > 0; I.back().pop_back())
                            unassign(I.back().back());
                        unassign(D.back()[0]);
                        D.back()[1] ^= 1;
                        D.back()[2] = 1;
                        UNIT.clear();
                        assign(D.back()[0], D.back()[1], 1);
                        //printf("~level=%d\n", level);
                        continue;
                    }
                    //printf("learned clause\n");

                    int maxlevel = 0;
                    for (int y : cut)
                        if (abs(y) != *tores.begin())
                            maxlevel = max(maxlevel, LEVEL[abs(y)]);
                    for (; D.size() > maxlevel + 1; D.pop_back(), I.pop_back())
                    {
                        for (; I.back().size() > 0; I.back().pop_back())
                            unassign(I.back().back());
                        unassign(D.back()[0]);
                    }
                    level = maxlevel;
                    m++;
                    C.push_back({});
                    int uip;
                    for (int y : cut)
                    {
                        C.back().push_back(y);
                        if (*tores.begin() == abs(y))
                            uip = y;
                    }
                    WAT.push_back({ 0, 1 });
                    //vector<int> STATt(n + 1, 0);
                    set<int> used;
                    for (int i = 0; i <= alevel; i++)
                    {
                        int z = aLOG[i];
                        //STATt[z] = 1;
                        used.insert(z);
                        for (int j = 0; j < 2; j++)
                        {
                            int &p = WAT.back()[j], &q = WAT.back()[j ^ 1];
                            if (p >= C.back().size())
                                continue;
                            int y = C.back()[p];
                            if (abs(y) != z || X[z] == y > 0)
                                continue;
                            int pt = p;
                            for (p = max(p, q); p < C.back().size(); p++)
                            {
                                int y = C.back()[p];
                                if (p != q && !(used.find(abs(y)) != used.end() && X[abs(y)] != y > 0))
                                    break;
                            }
                            LOG[i].push_back({ (int)C.size() - 1, j, pt, p });
                        }
                    }
                    for (int j = 0; j < 2; j++)
                    {
                        int &p = WAT.back()[j];
                        if (p < C.back().size())
                        {
                            int y = C.back()[p];
                            POS[abs(y)][y > 0].insert({ (int)C.size() - 1, j });
                        }
                    }
                    UNIT.clear();
                    UNIT.push_back({ uip, (int)C.size() - 1 });

                    //printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
                    if ((double)(clock() - t) / CLOCKS_PER_SEC > changetime)
                        return ss.str();
                }
            }
            if (!sat)
            {
                printf("~~~~~~~~~~~   unsat   ~~~~~~~~~~~\n");
                //fprintf(fp, "s UNSATISFIABLE\n");
                ss << "s UNSATISFIABLE\n";
                break;
            }
            if (err > -1)
            {
                //printf("err > -1\n");
                continue;
            }
            if (rem <= 0)
            {
                printf("level=%d\n", level);
                check();
                //fprintf(fp, "s SATISFIABLE\nv ");
                //for (int i = 1; i <= n; i++)
                //    fprintf(fp, "%d ", (X[i] ? i : -i));
                //fprintf(fp, "0\n");
                ss << "s SATISFIABLE\nv ";
                for (int i = 1; i <= n; i++)
                    ss << (X[i] ? i : -i) << " ";
                ss << "0\n";
                break;
            }
            for (int i = 0; i < n; i++)
            {
                //printf("bbb\n");
                int x = ORDER[i];
                if (STAT[x] == 0)
                {
                    level++;
                    int val = rand() & 1;
                    D.push_back({ x, val, 0 });
                    I.push_back({});
                    assign(x, val, 1);
                    break;
                }
            }
        }
        printf("n=%d m=%d\n", n, m);
        printf("done\n");
        return ss.str();
    }
};

int main(int argc, char **argv)
{
    const int SLICENUM = 5;
    const double CHANGETIME = 10;
    const double RESTARTTIME = 500;
    srand(12345);
    string instr = argv[1];
    string outstr = instr.substr(0, instr.length() - 3) + "sat";
    ofstream out(outstr);
    vector<vector<int>> C;
    int n;
    parse_DIMACS_CNF(C, n, instr.c_str());
    vector<SatSol> satsols(SLICENUM);
    clock_t begt = clock(), endt, t = begt;
    for (int slice = 0; slice < SLICENUM; slice++)
        satsols[slice].init(C, n);
    string ans;
    for (int slice = 0; ans.length() == 0; slice = (slice + 1) % SLICENUM)
    {
        if ((double)(clock() - t) / CLOCKS_PER_SEC > RESTARTTIME)
        {
            t = clock();
            printf("================restart==============\n");
            for (int i = 0; i < SLICENUM; i++)
            {
                vector<vector<int>> CC = satsols[i].C;
                printf("i=%d CC.size=%d\n", i, CC.size());
                satsols[i] = SatSol();
                satsols[i].init(CC, n);
            }
        }
        //ans = satsols[slice].sol(0.95, CHANGETIME);
        ans = satsols[slice].sol((double)slice / SLICENUM, CHANGETIME);
        printf("~~~returned from sol, slice=%d~~~\n", slice);
    }
    endt = clock();
    printf("elapsed time(clock): %f\n", (double)(endt - begt) / CLOCKS_PER_SEC);
    out << ans;
}
