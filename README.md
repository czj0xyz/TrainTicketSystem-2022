# TrainTicketSystem 开发文档

## 程序功能概述

### 基础功能 

* 程序设计了一个火车票订票系统，该系统向用户提供购票业务相关功能，包括车票查询、购票、订单操作等，以及向管理员提供后台管理功能。同时需要对用户数据、购票数据、车次数据进行本地存储，并对其进行高效操作。

* 程序通过标准输入输出进行交互，具体输入指令与输出格式见[此处](https://github.com/ACMClassCourse-2021/TicketSystem/blob/master/README.md)。

  程序在接受完整命令后应立即输出结果并再次进入等待输入状态。

* **登录栈**

  * 系统允许多个用户同时登录。

* **时间栈**

  * 每一个操作均有一个时间戳，且时间戳大小随操作次序严格递增，系统允许有时间回溯操作，即允许系统状态回到某个时间戳。

### 附加功能

* **数据库的导出 / 备份 / 增量备份**
  * 系统支持数据的导出、备份以及复制功能。(仍在考虑)

## 主体逻辑说明

### 运行

* 查看文件是否存在，若不存在则先建立文件。
* 构造Train_Management以及User_Management类。
* 根据交互命令执行操作，同时根据时间戳记录系统状态。
* 程序运行时保证有完整的鲁棒性。

### 实现细节

* 程序在首次运行时便创建用户名为``Root``，密码为``Root``，权限为``{10}``的根用户，用于创建其他用户。
* 记录每一个列车的出发地点以及出发时间，并以此为关键词做一个对应列车ID的映射。在``query_ticket``中可以直接查询``(From，date)``组成的pair对应的列车，然后暴力枚举观察是否列车能满足条件。(这一部分可以考虑使用线段树优化)
* 对于固定列车，用定长数组维护所有发车列车的剩余车票。(这一部分可以考虑使用线段树优化)
* 利用链表记录候票人，每次在有用户退票之后依次遍历链表检查是否有用户的需求得到满足。
* 使用专门的类``token_scanner``来处理读入的命令。
* 使用一个不定长数组以时间戳单调递增的方式记录所有命令，回溯时暴力重构。

## 储存文件结构

*注：每个文件以二进制方式储存，最小储存单元为一个 class，每次写入或读入一个 class，其长度可用 `sizeof()` 获得。注意：字符类型的情况下需用 `char[<length> + 1]` 而非 `std::string` 完成，否则无法正常读写。*

### 概览 

1. 用户信息
2. 列车信息
3. 操作序列

### 详细结构

1. **用户信息**
   * ``username``：由字母开头，由字母、数字和下划线组成的字符串，长度不超过 20。
   * ``password``：由可见字符组成的字符串，长度不低于 1，不超过 30。
   * ``name``：由 2 至 5 个汉字组成的字符串。
   * ``mailAddr``：同一般邮箱地址格式，且仅含数字、大小写字母，`@`和`.`，长度不超过 30。
   * `privilege`：为 0~10 中的一个整数。
2. **列车信息**
   * `trainID`：由字母开头，字母、数字和下划线组成的字符串，长度不超过 20。
   * `stationNum`：一个不低于 2 且不超过 100 的整数。
   * `stations`：共 `stationNum` 项，由汉字组成，不超过 10 个汉字。
   * `seatNum`：一个不超过 $10^5$ 的整数。
   * `prices`：共 `(stationNum - 1)` 项，每一项是一个不超过 $10^5$ 的整数。
   * `startTime`：格式为 `hh:mm` ，是`Date`类型。
   * `travelTimes`：共 `(stationNum - 1)` 项，每一项是一个不超过 $10^4$ 的整数。
   * `stopoverTimes`：，共 `(stationNum - 2)` 项，每一项为一个不超过 $10^4$ 的整数。
   * `saleDate`：有两个`Date`类型组成。
   * `type`：一个大写字母。
3. **操作序列**
   * 由一个字母、数字、下划线和-组成的字符串，长度不超过100。

## 代码结构及说明

### main.cpp

```c++
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

#include "Train.h"
#include "User.h"
#include "token_scanner"

vector<std::string>command;//记录指令

void Initialization();//检查文件是否存在并新建文件

void Process(char *S);//读取指令并执行 

void rollback(int time);//回溯

int main(){	
	return 0;
}
```

### token_scanner.h

```c++
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

class token_scanner{
private:
	std::string buffer;
	int point;
	char delimiter;

public:
	token_scanner();
	
	token_scanner(const std::string &buffer,const int &point,const char &delimiter);
	
	~token_scanner();
	
	std::string next_token();//返回下一个字符串 
	
	friend std::istream& operator>>(std::istream& input, TokenScanner& obj);//读入新的字符串 
};
```

### Train.h

```c++
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

#include "map.h"
#include "token_scanner.h"
#include "Date.h"

struct Waiting_Users{
	std::string UserID;
	int From,To,need;//购票起点、终点以及需要的票数
	Node *next;//链表的下一个节点 
};

class train {
private:
	char trainID[21];
	int stationNum;
	char **stations;
	int *prices,*travelTimes,*stopoverTimes;
	Date startTime,saleDate_L,saleDate_R;//saleDate_L是售票区间的左端点，saleDate_R是售票区间的右端点 
	char type;
    bool flag;//是否已发布
	
	map<std::string,int>pos;//从车站名到第几个车站的映射 
	Waiting_Users *head;//第i天发车的火车的车票候补人的链表 
	int **LeftTicket;//第i天发车的火车的第j张票还剩余几张 
	
public:
	train();
	
	train(const char &trainID_,const int &stationNum_,const char **stations_,const int *prices_,const int *travelTimes_,
		  const int *stopoverTimes_,const Date &startTime_,const Date &saleDate_L_,const Date &saleDate_R_,const char &type_);
	
	~train();
	
	void Update_ticket(const Date &date,const int &l,const int &r,const int &num);//更新车票，从date发车的火车有num张从l到r的车票被买了 
	
	int query_ticket(const Date &date,const int &l,const int &r);//询问从date发车的火车至多能买多少张从l到r的车票
	
	void check_ticket(const Date &date);//在某一用户退票后，需确认有哪些候票人能拿到票 
};

class TrainManagement{
private:
    bpt<std::string,train> file_train("file_train","file_train_delete");
	map::map<std::string,train> Train;//从trainID到具体火车的映射 
	map::map<pair<std::string,Date>,vector<std::string> >start;//从(trainID,出发时间)到具体火车的映射 
	
public:
	TrainManagement();
	
	~TrainManegement();
	
	void add_train(const train &New_train);//add_train command 
	
	void release_train(const std::string &trainID);//release_train command 
	
	void query_train(const std::string &trainID,const Date &date);//query_train command 
	
	void query_ticket(const std::string &From,const std::string &To,const Date &date,bool flag);//query_ticket command 
	
	void query_transfer(const std::string &From,const std::string &To,const Date &date,bool flag);//query_transfer command 
	
};
```

### User.h

```c++
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

#include "token_scanner.h"
#include "map.h"

struct Ticket{
	std::string trainID,From,To;
	Date date;
	int num;
};

class User{
private:
    char username[22];
    char password[32];
    char name[12];
    char mailAddr[32];
    int privilege;
	
	vector<Ticket>Purchased;//记录用户已买的票 
public:
	User();
	
	User(const char *username_,const char *password_,const char *name_,const char *mailAddr_,const int &privilege_);
	
	~User();
	
	void buy_ticket(const string &TrainID,const Date &date,const string &From,const string &To,const int &need);//buy_ticket command 
	
	void query_order();//query_order command 
	
	void refund_ticket(const int &place);//refund_ticket command 
};

class User_Management{
private:
    bpt<std::string,User> file_user("file_user","file_user_delete");
	map<std::string,User>pos;//UserID到用户的映射 
public:
	User_Management();
	
	~User_Management();
	
    void add_user(const std::string &cur_username,const std::string &username,const std::string &pwd,const std::string &name,
	 			  const std::string &mailAddr,const int &privilege);//add_user command 
    
    void login(const std::string &username,const std::string &pwd);//login command 
    
    void logout(const std::string &username);//logout command 
    
    void query_profile(const std::string &cur_username,const std::string &username);//query_profile command 
    
    void modify_profile(const std::string &cur_username,const std::string &username,const std::string &pwd,const std::string &name,
	 			  const std::string &mailAddr,const int &privilege);//modify_profile command 
};
```

### Bplus_Tree.h

```c++
template<class Key, class T, int M = 30, class Compare= std::less<Key> >
    class bpt {
        public:
        typedef pair<Key, T> value_type;//STLite 自带pair
        struct node {
            bool is_leave = 0;//是否是叶节点
            int length = 0;//当前长度
            long long address = 0;//外存的地址
            long long father = -1;//父亲的地址
            value_type value[M];
            long long son[M];//M个儿子的地址
            long long pre = -1;//用于记录叶子节点的前驱和后继
            long long next = -1;
        };
        node root;//根节点
        std::fstream file;//主体的文件
        std::fstream file_delete;//用于空间回收的文件
        std::string file_name;
        std::string file_delete_name;//文件的名字
        Compare cpy;//比较函数

        explicit bpt(std::string file_name_, std::string file_delete_name_);//用于初始化文件

        void insert(const value_type &value);

        T find(const Key &key);

        void erase(const Key &key);
    }
class Calendar
{   int month,day,hour,min;
    constexpr static const int num[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    constexpr static const int leap_num[13]={0,31,29,31,30,31,30,31,31,30,31,30,31};
    constexpr static const int sum_num[13]={0,31,59,90,120,151,181,212,243,273,304,334,365};
    constexpr static const int sum_leap_num[13]={0,31,60,91,121,152,182,213,244,274,305,335,366};
public:
    friend int GetMinNum(const Calendar &cal);
    friend Calendar GetMin(int x);
    Calendar();
    Calendar(std::string s);
    Calendar(const int &year_,const int &month_,const int &day_);
    Calendar &operator=(const Calendar &cal);
    Calendar &operator+=(const int &x);
    Calendar &operator-=(const int &x);
    friend Calendar operator+(Calendar cal,const int &x);
    friend Calendar operator-(Calendar cal,const int &x);
    friend int operator-(const Calendar &cal_1,const Calendar &cal_2);
    friend bool operator==(const Calendar &cal_1,const Calendar &cal_2);
    friend bool operator!=(const Calendar &cal_1,const Calendar &cal_2);
    friend bool operator>=(const Calendar &cal_1,const Calendar &cal_2);
    friend bool operator>(const Calendar &cal_1,const Calendar &cal_2);
    friend bool operator<=(const Calendar &cal_1,const Calendar &cal_2);
    friend bool operator<(const Calendar &cal_1,const Calendar &cal_2);
    Calendar &operator++();
    Calendar operator++(int x);
    Calendar &operator--();
    Calendar operator--(int x);
    friend std::ostream &operator<<(std::ostream &os,const Calendar &cal);
    friend std::istream &operator>>(std::istream &is,Calendar &cal);
};
```