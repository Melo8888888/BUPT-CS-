#include "CFG_Simplify.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

// �����Ų���ʽ
void eli_epsilon(set<char> &N, set<char> &T, map<char, std::set<std::string>> &P, char &S)
{
    // ��һ�����ҵ����п���ֱ���Ƴ� �� �ķ��ս��
    std::set<char> canEpsilon;
    for (auto it = P.begin(); it != P.end(); ++it)
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            if (*it2 == "#")
            {
                canEpsilon.insert(it->first);
                break;
            }

    // �ڶ����������ҵ��µĿ����Ƴ� �� �ķ��ս����ֱ�����ϲ��ٱ仯
    while (true)
    {
        auto old_size = canEpsilon.size();
        for (auto it = P.begin(); it != P.end(); ++it)
        {
            // ��ǰ�ַ��Ѿ��ڼ����У�����
            if (canEpsilon.find(it->first) != canEpsilon.end())
                continue;

            for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            {
                bool canEpsilonFlag = true;
                for (auto it3 = it2->begin(); it3 != it2->end(); ++it3)
                {
                    // ��ǰ�ַ������Ƴ� ��
                    if (canEpsilon.find(*it3) == canEpsilon.end())
                    {
                        canEpsilonFlag = false;
                        break;
                    }
                }

                // ����ʽ�е������ַ������Ƴ� �ţ���ǰ�ַ�Ҳ���Ƴ� ��
                if (canEpsilonFlag)
                {
                    canEpsilon.insert(it->first);
                    break;
                }
            }
        }

        if (old_size == canEpsilon.size())
            break; // ���ϲ��ٱ仯���˳�ѭ��
    }

    // �������������µķ��ս�����������µĲ���ʽ���� P1
    std::map<char, std::set<std::string>> P1;

    for (auto it = P.begin(); it != P.end(); ++it)
    {
        std::set<std::string> new_productions; // �µĲ���ʽ�б�

        // �����ɲ���ʽ�б���ÿһ������ʽ
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            int count_epsilon = 0; // ��¼��ʽ�п��¿յķ��ս������
            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3)
                if (canEpsilon.find(*it3) != canEpsilon.end())
                    ++count_epsilon; // ��ǰ�����ǿ��¿յ�

            // ö�ٿ��¿յķ��ս����������ϣ�2�� n �η���
            for (int i = 0; i < (1 << count_epsilon); ++i)
            {
                std::string new_production; // �µĲ���ʽ
                int charac_index = 0;       // ��ǰ�����Ŀ��¿��ַ��ı��
                for (auto it3 = it2->begin(); it3 != it2->end(); ++it3)
                {
                    // ����ǰ�ַ����Ƴ� ��
                    if (canEpsilon.find(*it3) != canEpsilon.end())
                    {
                        if ((i >> charac_index) & 1)        // ��ǰ�ַ��ڵ�ǰ����ʽ�б�ѡ��
                            new_production.push_back(*it3); // �����µĲ���ʽ
                        ++charac_index;
                    }
                    else
                        // ����ǰ�ַ������Ƴ� ��
                        new_production.push_back(*it3);
                }
                if (new_production.size() > 0 && new_production != "#") // ����µĲ���ʽ�ǿ��Ҳ��ǿմ�����ʽ
                    new_productions.insert(new_production);             // �����µĲ���ʽ�б�
            }
        }

        P1[it->first] = new_productions; // �����µĲ���ʽ����
    }

    // �����ʼ���ſ����Ƴ� �ţ�����Ҫ����һ���µ���ʼ����
    if (canEpsilon.count(S) > 0)
    {
        S = NEW_STARTER;
        P1[NEW_STARTER] = {"#", "S"};
        N.insert(NEW_STARTER);
    }

    P = P1;
}

// ����������ʽ
void eli_single(set<char> &N, set<char> &T, map<char, std::set<std::string>> &P, char &S)
{
    // �����µĲ���ʽ����
    std::map<char, std::set<std::string>> new_P;

    // �����ɵĲ���ʽ�����е�ÿ�����ս��A
    for (auto A : N)
    {
        // ��ʼ���µĲ���ʽ�б����µķ��ս������
        std::set<char> new_N;
        new_P[A] = {};
        new_N.insert(A);

        // �ҳ����п���ֱ���Ƴ��������ս���Ĳ���ʽ��������Щ���ս������new_N
        for (auto it = P[A].begin(); it != P[A].end(); ++it)
            if (it->size() == 1 && N.find(it->at(0)) != N.end())
                new_N.insert(it->at(0));

        // ������չnew_Nֱ�����ٱ仯
        std::set<char> prev_N;
        do
        {
            prev_N = new_N;
            for (auto B : prev_N)
                for (auto it = P[B].begin(); it != P[B].end(); ++it)
                    if (it->size() == 1 && N.find(it->at(0)) != N.end())
                        new_N.insert(it->at(0));
        } while (new_N != prev_N);

        // �����µĲ���ʽ�б�
        for (auto B : new_N)
            for (auto it = P[B].begin(); it != P[B].end(); ++it)
                if (it->size() > 1 || T.count(it->at(0)) > 0 || it->at(0) == EPSILON) // ����ǰ����ʽ���ǵ�����ʽ
                    new_P[A].insert(*it);

        // ȥ���ظ��Ĳ���ʽ�����µĲ���ʽ�б�Ϊ�գ������ A --> #
        if (new_P[A].size() == 0)
            new_P[A].insert("#");
    }

    // ����P
    P = new_P;
}

// �������÷���
void eli_useless(set<char> &N, set<char> &T, map<char, std::set<std::string>> &P, char &S)
{
    // ��һ�������ɷ��ż�
    std::set<char> N0, N1;
    // ��������ʽ P
    for (auto it = P.begin(); it != P.end(); ++it)
        // ��������ʽ�Ҳ�
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            bool in_T = true;
            // ����Ҳ�����Ƿ��� T ��
            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3)
            {
                if (T.find(*it3) == T.end())
                {
                    in_T = false;
                    break;
                }
            }
            // ����Ҳ���Ŷ��� T �У������ N1
            if (in_T)
            {
                N1.insert(it->first);
                break;
            }
        }

    // ������չN1��ֱ��N1���ٱ仯
    while (N0 != N1)
    {
        N0 = N1;
        for (auto it = P.begin(); it != P.end(); ++it)
            for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            {
                bool in_N0 = true;
                // ����Ҳ�����Ƿ��� N0 ��
                for (auto it3 = it2->begin(); it3 != it2->end(); ++it3)
                {
                    if (N0.find(*it3) == N0.end() && T.find(*it3) == T.end())
                    {
                        in_N0 = false;
                        break;
                    }
                }
                // ����Ҳ���Ŷ��� N1 �У������ N1
                if (in_N0)
                {
                    N1.insert(it->first);
                    break;
                }
            }
    }

    // ���·��ս������ N
    N = N1;

    // �����µĲ���ʽ���� P1
    std::map<char, std::set<std::string>> P1;
    for (auto it = P.begin(); it != P.end(); ++it)
    {
        if (N.find(it->first) == N.end())
            continue;

        P1[it->first] = {};
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            // �������ʽ�Ҳ�ķ��Ų���N��T�У���in_N��Ϊfalse
            bool in_N = true;
            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3)
                if (*it3 != EPSILON && N.find(*it3) == N.end() && T.find(*it3) == T.end())
                {
                    in_N = false;
                    break;
                }

            // �������ʽ�Ҳ�ķ��Ŷ���N��T�У��򽫲���ʽ����P1
            if (in_N)
                P1[it->first].insert(*it2);
        }
    }

    // ���²���ʽ����PΪP1
    P = P1;

    // �ڶ������ɴ���ż����Ƚ���ʼ���ŷŽ��ɴ���ż�
    std::set<char> OK_N = {S};
    for (auto it = P[S].begin(); it != P[S].end(); ++it)
        for (auto it2 = it->begin(); it2 != it->end(); ++it2)
            OK_N.insert(*it2);

    // ������չnew_N��ֱ��new_N���ٱ仯
    while (N0 != OK_N)
    {
        N0 = OK_N;
        for (auto A : N0)
        {
            if (N.find(A) == N.end())
                continue;
            for (auto it = P[A].begin(); it != P[A].end(); ++it)
                for (auto it2 = it->begin(); it2 != it->end(); ++it2)
                    OK_N.insert(*it2);
        }
    }

    // ���·��ս������N���ս������T
    for (auto it = N.begin(); it != N.end();)
    {
        if (OK_N.count(*it) == 0)
            it = N.erase(it);
        else
            ++it;
    }
    for (auto it = T.begin(); it != T.end();)
    {
        if (OK_N.count(*it) == 0)
            it = T.erase(it);
        else
            ++it;
    }

    // �����µĲ���ʽ����P2
    std::map<char, std::set<std::string>> P2;
    for (auto it = P.begin(); it != P.end(); ++it)
    {
        // ����ʽ����������
        if (N.find(it->first) == N.end())
            continue;

        P2[it->first] = {};
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            // �������ʽ�Ҳ�ķ��Ŷ���N��T�У��򽫲���ʽ����P2
            bool in_N = true;
            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3)
                if (*it3 != EPSILON && N.find(*it3) == N.end() && T.find(*it3) == T.end())
                {
                    in_N = false;
                    break;
                }

            if (in_N)
                P2[it->first].insert(*it2);
        }
    }

    // ���²���ʽ����PΪP2
    P = P2;
}

// ��ӡ�򻯺�Ľ��
void printout(set<char> &N, set<char> &T, map<char, std::set<std::string>> &P, char &S)
{
    std::cout << "The result after simplify:\n";
    // ���ս������
    std::cout << "N={";
    for (auto it = N.begin(); it != N.end(); ++it)
    {
        std::cout << *it;
        if (std::next(it) != N.end()) // ����������һ��Ԫ�أ��������
            std::cout << ", ";
    }
    std::cout << "}\n";

    // �ս������
    std::cout << "T={";
    for (auto it = T.begin(); it != T.end(); ++it)
    {
        std::cout << *it;
        if (std::next(it) != T.end()) // ����������һ��Ԫ�أ��������
            std::cout << ", ";
    }
    std::cout << "}\n";

    // ����ʽ����
    std::cout << "P:\n";
    for (auto it = P.begin(); it != P.end(); ++it)
    {
        std::cout << "\t" << it->first << " --> ";
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            if (std::next(it2) != it->second.end()) // ����������һ��Ԫ�أ��������
                std::cout << *it2 << " | ";
            else
                std::cout << *it2 << "\n";
        }
    }

    // ��ʼ����
    std::cout << "S = " << S << "\n";
}