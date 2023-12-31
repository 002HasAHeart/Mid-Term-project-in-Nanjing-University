#include<stdio.h>                                  // 1. 赋值后更新值 2. 表达式给变量赋值  3. 再优化 eval函数
#include<string.h>
#include<stdlib.h>     //atoi   atof
#define N 1005

typedef struct value           //三种输出的内容
{
    enum
    {
        INT,
        FLOAT,
        ERROR
    }type;
    union
    {
        int iVla;
        double fVla;
    }val;
}Value;

typedef struct token     // 读入--
{
    int type;
    char str[32];
}Token;

Token tokens[N], copy[N];                 //copy 数组是为了寻找主运算符
char token[N];
int cnt;                                 //这是tokens 数组的全局脚标

typedef struct assignment   //赋值的结构体数组
{
    Value ans;
    char name[32];
}Assignment;

Assignment assign[N];
int CNT;                                 //这是assign数组（赋值）的全局脚标

int check_integer(const char *s);             //词法分析，应该没有问题
int check_variable(const char *s);
int check_operator(const char *s);
int check_double(const char *s);
int check_op(int l, int r);
int check_parentheses(int l, int r);        // 检查是否有包裹起来的括号（采用了括号序列中的栈实现）
int check_minus(int l, int r);             //解引用括号，找到主运算符时返回0
Value minus(Value a);
int find(int l, int r);                     //寻找主运算符
int check(int l, int r);                    //是否有赋值
Value eval(int l, int r);                   //芝士求值函数
Value evalassign(int l, int r);             //赋值函数
void print(Value v);                       //统一的输出函数

char stack[N];                          // 栈 不用多说
int top;
char pop(char a[])
{
    return a[--top];
}
void push(char a[],char id)
{
    a[top++]=id;
}

int main()
{
    while(gets(token))              //使用gets读入， 并以空格为分界分割 (strtok)函数
    {
        cnt = 0;              //芝士角标
        int flag = 0;
        memset(tokens,'\0',sizeof(tokens));         //清空结构体
        memset(copy, '\0', sizeof(copy));
        char *temp = strtok(token," ");
        while(temp)
        {
            if(check_integer(temp))
            {
                tokens[cnt].type = 1;
                strcpy(tokens[cnt++].str,temp);
            }
            else if(check_variable(temp))
            {
                tokens[cnt].type = 2;
                strcpy(tokens[cnt++].str,temp);
            }
            else if(check_operator(temp))
            {
                tokens[cnt].type = 3;
                strcpy(tokens[cnt++].str,temp);
            }
            else if(check_double(temp))
            {
                tokens[cnt].type = 4;
                strcpy(tokens[cnt++].str,temp);
            }
            else
            {
                flag = 1;           //不属于任何一种token
            }
            temp = strtok(NULL," ");
        }
        if(flag)
        {
            printf("Error\n");
            continue;                                         //碰到词法错误或者赋值就把求值的部分continue过去
        }
        if(check(0, cnt - 1))
        {
            print(evalassign(0, cnt - 1));
            continue;
        }
        print(eval(0, cnt - 1));
    }
    return 0;
}

int check_op(int l, int r)
{
    for(int i = l; i <= r; i++)
    {
        if(tokens[i].str[0] == '+' || tokens[i].str[0] == '/' || tokens[i].str[0] == '*' || tokens[i].str[0] == '-')
            return 1;
    }
    return 0;
}

int check(int l, int r)
{
    for(int i = l; i <= r; i++)
    {
        if(tokens[i].str[0] == '=')
            return 1;
    }
    return 0;
}

Value evalassign(int l, int r)                //赋值   不会有括号
{
    for(int i = 0; i <= cnt; i++)
    {
        copy[i] = tokens[i];
    }
    Value output;
    for(int j = l; j <= r; j++)
    {
        if(copy[j].str[0] == '=')
        {
            for(int k = j + 1; k <= r; k++)
            {
                if(copy[k].str[0] == '=')
                {
                    if(check_op(j, k))
                    {
                        output.type = ERROR;
                        return output;
                    }
                    else
                    {
                        copy[k].str[0] = copy[j].str[0] = '\0';
                    }
                }
            }
        }
    }
    for(int i = l; i <= r; i++)
    {
        if(tokens[i].str[0] == '=' && tokens[i - 1].type == 2)
        {
            strcpy(assign[CNT].name, tokens[i - 1].str);
            assign[CNT++].ans = evalassign(i + 1, r);
            output = assign[CNT - 1].ans;
            return output;
        }
        else if(tokens[i].str[0] == '=' && tokens[i - 1].type != 2)
        {
            output.type = ERROR;
            return output;
        }
    }
    return eval(l, r);
}

int check_integer(const char *s)            //type 1         注意没有前导0
{
    int len1 = strlen(s);
    int flag1 = 1;
    if(len1 == 1)
    {
        if(*s >= '0' && *s <= '9')
            flag1 = 1;
        else
            flag1 = 0;
    }
    else if(strchr(s,'.') == NULL && *s >= '1' && *s <='9' && len1 > 1)
    {
        for(int i = 1; i < len1 ; i++)
        {
            if(!(*(s + i) >= '0' && *(s + i) <= '9'))
                flag1 = 0;
        }
    }
    else
    {
        flag1 = 0;
    }
    return flag1;
}

int check_variable(const char *s)            //type 2
{
    int flag2 = 1;
    int len2 = strlen(s);
    if(!(*s >= '0' && *s <= '9'))
    {
        for(int i = 0; i < len2; i++)
        {
            if(!((*(s + i) >= 'a' && *(s + i) <= 'z') || (*(s + i) >= 'A' && *(s + i) <= 'Z') || *(s + i) == '_' || (*(s + i) >='0' && *(s + i) <= '9')))
            {
                flag2 = 0;
            }
        }
    }
    else
    {
        flag2 = 0;
    }
    return flag2;
}

int check_operator(const char *s)            //type 3
{
    if (!strcmp(s, "+") || !strcmp(s, "-") || !strcmp(s, "*") || !strcmp(s, "/") || !strcmp(s, "(") ||
        !strcmp(s, "=") || !strcmp(s, ")"))
        return 1;
    return 0;
}

int check_double(const char *s)              //type 4
{
    if(*s == '0' && *(s + 1) != '.')
    {
        return 0;
    }
    int flag3 = 1;
    int cnt1 = 0;
    int temp = 0;
    int len3 =strlen(s);
    for(int i = 0;i < len3; i++)
    {
        if(*(s + i) == '.')
        {
            temp = i;
            cnt1++;
        }
    }
    if(cnt1 == 1 && temp != 0 && temp != len3 - 1 )
    {
        for(int i = 0; i < len3; i++)
        {
            if(!((*(s + i) >= '0' && *(s + i) <= '9') || *(s + i) == '.'))
                flag3 = 0;
        }
    }
    else
    {
        flag3 = 0;
    }
    return flag3;
}

int check_parentheses(int l, int r)      // l < r   栈(括号序列)
{
    memset(stack, '\0', sizeof(stack));
    top = 0;
    if(tokens[l].str[0] == '(' && tokens[r].str[0] == ')')
    {
        for (int j = l + 1; j <= r - 1; j++)
        {
            if (!strcmp(tokens[j].str, "("))
            {
                push(stack, '(');
            }
            if (!strcmp(tokens[j].str, ")"))
            {
                if (stack[top - 1] == '(')
                    pop(stack);
                else
                    push(stack, ')');
            }
        }
    }
    else
    {
        return 0;
    }
    if(top == 0)
        return 1;
    return 0;
}

int check_minus(int l, int r)
{
    if(tokens[l].str[0] != '-')
        return 0;
    if(find(l, r))     //找到了主运算符
        return 0;
    else
        return 1;
}

Value minus(Value a)
{
    a.val.iVla = -a.val.iVla;
    a.val.fVla = -a.val.fVla;
    return a;
}

int find(int l, int r)
{
    int ans = 0;
    //这里把负号删掉
    for(int j = l; j <= r; j++)
    {
        if(copy[j].str[0] == '-')
        {
            if((copy[j - 1].str[0] == ')' || copy[j - 1].type == 1 || copy[j - 1].type == 2 || copy[j - 1].type == 4)&&j - 1 >= l)
            {
                ;//啥也不做，它是真的减号
            }
            else
            {
                copy[j].str[0] = '\0';  //删掉负号
            }
        }
    }
    for(int k = l; k <= r; k++)     //将出现在成对括号中的运算符（它们肯定不是主运算符）消除
    {
        if(copy[k].str[0] == '(')
        {
            for(int m = k; m <= r; m++)
            {
                if(copy[m].str[0] == ')')
                {
                    if(check_parentheses(k, m))
                    {
                        for(int n = k; n <= m; n++)
                        {
                            if(copy[n].type == 3)
                                copy[n].str[0] = '\0';
                        }
                    }
                }
            }
        }
    }
    for(int i = l; i <= r; i++)
    {
        if (copy[i].str[0] == '*' || copy[i].str[0] == '/')
            ans = i;
    }
    for(int i = l; i <= r; i++)
    {
        if (copy[i].str[0] == '+' || copy[i].str[0] == '-')
            ans = i;
    }
    return ans;
}

Value eval(int l, int r)
{
    Value ans;
    for(int i = 0; i <= cnt; i++)
    {
        copy[i] = tokens[i];
    }
    if(l > r)
    {
        ans.type = ERROR;
        return ans;
    }
    else if(l == r)
    {
        if(tokens[l].type == 1)
        {
            ans.type = INT;
            ans.val.iVla = atoi(tokens[l].str);
            return ans;
        }
        else if(tokens[l].type == 4)
        {
            ans.type = FLOAT;
            ans.val.fVla = atof(tokens[l].str);
            return ans;
        }
        else if(tokens[l].type == 2)
        {
            if(CNT == 0)
            {
                ans.type = ERROR;
                return ans;
            }
            for(int i = CNT; i >= 0; i--)
            {
                if(!strcmp(assign[i].name, tokens[l].str) && assign[i].ans.type != ERROR)
                {
                    if(assign[i].ans.val.iVla == 0 && assign[i].ans.val.fVla == 0)
                    {
                        ans.type = ERROR;
                        return ans;
                    }
                    ans = assign[i].ans;
                    return ans;
                }
            }
            ans.type = ERROR;
            return ans;
        }
        else
        {
            ans.type = ERROR;
            return ans;
        }
    }
    else if(check_parentheses(l, r))
    {
        return eval(l + 1, r - 1);
    }
    else if(check_minus(l, r))
    {
        return minus(eval(l + 1, r));
    }
    else
    {
        for(int i = 0; i <= cnt; i++)
        {
            copy[i] = tokens[i];
        }
        int op = find(l, r);
        if(op == 0)
        {
            ans.type = ERROR;
            return ans;
        }
        Value val1 = eval(l, op - 1);
        Value val2 = eval(op + 1, r);
        if(val1.type == ERROR || val2.type == ERROR)
        {
            ans.type = ERROR;
            return ans;
        }
        switch(tokens[op].str[0])         //without bonus;
        {
            case '+':
                if(val1.type == INT && val2.type == INT)
                {
                    ans.type = INT;
                    ans.val.iVla = val1.val.iVla + val2.val.iVla;
                }
                else
                {
                    ans.type = FLOAT;
                    ans.val.fVla =( (val1.type == INT) ? val1.val.iVla : val1.val.fVla ) + ( (val2.type == INT ) ? val2.val.iVla : val2.val.fVla );
                }
                return ans;
            case '-':
                if(val1.type == INT && val2.type == INT)
                {
                    ans.type = INT;
                    ans.val.iVla = val1.val.iVla - val2.val.iVla;
                }
                else
                {
                    ans.type = FLOAT;
                    ans.val.fVla =( (val1.type == INT) ? val1.val.iVla : val1.val.fVla ) - ( (val2.type == INT ) ? val2.val.iVla : val2.val.fVla );
                }
                return ans;
            case '*':
                if(val1.type == INT && val2.type == INT)
                {
                    ans.type = INT;
                    ans.val.iVla = val1.val.iVla * val2.val.iVla;
                }
                else
                {
                    ans.type = FLOAT;
                    ans.val.fVla =( (val1.type == INT) ? val1.val.iVla : val1.val.fVla ) * ( (val2.type == INT ) ? val2.val.iVla : val2.val.fVla );
                }
                return ans;
            case '/':
                if(val1.type == INT && val2.type == INT)
                {
                    ans.type = INT;
                    ans.val.iVla = val1.val.iVla / val2.val.iVla;
                }
                else
                {
                    ans.type = FLOAT;
                    ans.val.fVla =( (val1.type == INT) ? val1.val.iVla : val1.val.fVla ) / ( (val2.type == INT ) ? val2.val.iVla : val2.val.fVla );
                }
                return ans;
        }
    }
}

void print(Value v)
{
    switch (v.type)
    {
        case INT:
            printf("%d\n",v.val.iVla);
            break;
        case FLOAT:
            printf("%.6f\n",v.val.fVla);
            break;
        case ERROR:
            printf("Error\n");
            break;
    }
}