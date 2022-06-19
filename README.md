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

* 构造Train_Management以及User_Management类。
* bpt查看文件是否存在，若不存在则先建立文件。
* 根据交互命令执行操作，同时根据时间戳记录系统状态。
* 程序运行时保证有完整的鲁棒性。

### 实现细节

* 加入列车时，考虑维护`(station,trainID)->Station`的映射，其中`Station`包括了该站的到达以及出发时间(以第一天发车为准)、从起始站到该站的价格以及该站在列车中对应的位置。``buy_ticket``直接查出`Station`即可获得火车票的信息，`query_ticket`以及`query_transfer`查询pair第一维是`From`和`To`的所有满足条件的`Station`。
* `query_transfer`中先将第一辆列车所有的站放到哈希表内，再暴力枚举第二辆列车。询问过的列车也要放到哈希表中。
* 对于固定列车，用定长数组维护所有发车列车的剩余车票。
* 利用`((tarinID,kth),timestamp)`来记录每辆列车第`kth`次发车的候票人，每次在有用户退票之后依次遍历检查是否有用户的需求得到满足。
* 在`bpt`中`string`的存储要利用其`hash`值。
* 在`bpt`中加入缓存，即先被访问过的`value_type`先被存进哈希表中，再访问时从内存中进行访问。一些基本信息也可以考虑在内存中存储。
* `bpt`的`find`函数返回一个`(flag,T)`，`flag`表示是否找到了对应的`Key`值，这样可以减少一次`count`的访问。
* 用专门的类token_scanner来处理读入的命令。
* 使用专门的类``token_scanner``来处理读入的命令。
* 利用栈存储所有涉及`insert`、`erase`以及`modify`的操作，在`rollback`中弹出栈顶进行反向修改。

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
#include "token_scanner.h"
#include "string.hpp"

void Add_train();

void Buy_ticket(const std::string &username_,const std::string &trainID_,const Date &date,const int &num,const std::string &fr,const std::string &to,const std::string &flag);

void Refund_ticket(const std::string &username_,const std::string &kth);

void Query_order(const std::string &username_);

void Reset();

void Process();

int main(){
    Process();
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
	char buffer[10005];
	int sz,point;
	char delimiter;

public:
	token_scanner();
	
	~token_scanner();
	
	std::string next_token();
	
	std::string all_token();
	
	void change(const char &c);
	
	bool read();
};
```

### Train.h

```c++
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

#include "string.hpp"
#include "vector.hpp"
#include "Ticket.h"
#include "linked_hashmap.hpp"

class TrainManagement;

struct Basictrain{
    int maxday,maxseat;
    bool rel;
    Basictrain(){}

    Basictrain(const int &day,const int &seat,const int Rel=0);
};

struct Stations {
    int pos,pri;
    Date ari,lef;
    Stations(){}
    
    Stations(const int &pos_,const int &pri_,const Date &ari_,const Date &lef_);
};

class train{
private:
    sjtu::string trainID;
    int stationNum,seatNum;
	sjtu::string stations[M];
	int prices[M],travelTimes[M],stopoverTimes[M];
	Date saleDateL,saleDateR;
	char Type; 
	bool rel; 
public:
	train(){}
	
	train(const sjtu::string &trainID_,const int &stationNum_,const sjtu::string *stations_,const int &seatNum_,const int *prices_,const int *travelTimes_,
		  const int *stopoverTimes_,const Date &startTime_,const Date &saleDateL_,const Date &saleDateR_,const char &type_);

    inline Stations get_station(const int &rk);

    friend TrainManagement;
};

class Left_Ticket {
private:
    int data[M][M];

public:
    Left_Ticket() {}

    Left_Ticket(const int &stationNum, const int &seat);

    inline void Update_ticket(const int &l, const int &r, const int &num);

    inline int query_ticket(const int &l, const int &r);

    friend class TrainManagement;
};

class TrainManagement {
private:   
    sjtu::linked_hashmap<size_t, Basictrain > bas;
    sjtu::linked_hashmap<size_t, sjtu::string> trainid;

    sjtu::bpt<size_t, train>tra;
    sjtu::bpt<sjtu::pair<size_t,int> , Left_Ticket> Tk;
    sjtu::bpt<type3, Stations> sta;

    sjtu::bpt<type1, Ticket> Get_ticket;
    sjtu::bpt<sjtu::pair<type1,int>, Pendtype> inqu_ticket;
    sjtu::bpt<size_t, int> Num;

public:
    TrainManagement();

    ~TrainManagement();

    void add_train(const std::string &trainID_, const int &stationNum_, const sjtu::string *stations_, const int &seatNum,
              const int *prices_, const int *travelTimes_,
              const int *stopoverTimes_, const Date &startTime_, const Date &saleDateL_, const Date &saleDateR_,
              const char &type_);

    void release_train(const std::string &trainID, const int &timestamp);

    void delete_train(const std::string &trainID);

    void query_train(const std::string &trainID, const Date &date);
	
    void query_transfer(const std::string &From, const std::string &To, const Date &date,
                        bool flag);

    void Reset();

    void query_ticket(const std::string &From, const std::string &To, const Date &date, bool flag);
  
    void buy_ticket(const std::string &username,const std::string &trainID_, const Date &date, const std::string From_,
                                 const std::string To_,const int &ask,const bool &flag,const int &timestamp);

    void query_order(const size_t &username);
    
    void refund_ticket(const size_t &username, const int &k);
    
	void Rollback(const int &time);
};
```

### User.h

```c++
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

#include "utility.hpp"
#include "bpt.h" 
#include "linked_hashmap.hpp"

class User_Management;

struct User{
    char username[22];
    char password[32];
    char name[22];
    char mailAddr[32];
    int privilege;
	User();
	
	User(const std::string &username_,const std::string &password_,const std::string &name_,const std::string &mailAddr_,const int &privilege_);
	
	~User();
	
};

class User_Management{
private:
	sjtu::bpt<size_t,User,340,34,1> pos;
	sjtu::linked_hashmap<size_t,bool>log;
public:
	User_Management();

	~User_Management();
	
    void add_user(const std::string &cur_username,const std::string &username,const std::string &pwd,const std::string &name,
	 			  const std::string &mailAddr,const int &privilege);
    
    void login(const std::string &username,const std::string &pwd);
    
    void logout(const std::string &username);
    
    void query_profile(const std::string &cur_username,const std::string &username);
    
    void modify_profile(const std::string &cur_username,const std::string &username,const std::string &pwd,const std::string &name,
	 			  const std::string &mailAddr,const int &privilege);
	 							
	void Reset();

	void Rollback(const int &time);
};
```

### Ticket.h

```c++
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

#include "utility.hpp"
#include "string.hpp"
#include "Date.h"

struct Ticket2{
    sjtu::string trainID;
    Date TimeL,TimeR;
    int cost,time,num;
    Ticket2();

    Ticket2(const sjtu::string &id, const Date &l, const Date &r,
           const int &cost_, const int &Num, const int tm );

    void print(const std::string &f,const std::string &t);
};

struct Ticket {
    sjtu::string trainID, From, To;
    Date TimeL, TimeR; 
    int cost, time, num;

    Ticket() ;

    Ticket(const Ticket &res);

    Ticket(const sjtu::string &id, const sjtu::string &f, const sjtu::string &t, const Date &l, const Date &r,
           const int &cost_, const int &Num, const int tm = 0);

    void print();
};

struct Transfer_Ticket {
    Ticket A, B;
    int time, cost;

    Transfer_Ticket() {};
};

struct Pendtype{
    size_t username;
    int num,l,r,kth;
    Pendtype(){}

    Pendtype(const size_t &username_,const int &num_,const int &l_,const int &r_,const int &k_);
};

```

### Bplus_Tree.h

```c++
template<class Key, class T, int M = 100, int N = 300, int mm = 4, class Hash=std::hash<Key>, class Compare= std::less<Key> >
class bpt {
private:
    typedef pair<Key, T> value_type;
    struct node {
        bool is_leave = 0;
        int length = 0;
        int address = 0;
        int father = -1;
        Key value[M];
        int son[M];
        int leave_address = -1;
    };
    struct node_leaves {
        int length = 0;
        value_type value[N];
        int pre = -1;
        int next = -1;
        int address = -1;
        int key_address = -1;
        int father;
    };
    node root;
    sjtu::linked_hashmap<Key, T, Hash> hs;
    std::fstream file;
    std::fstream file_delete;
    std::fstream file_leaves;
    std::fstream file_delete_leaves;
    std::string file_name;
    std::string file_delete_name;
    std::string file_leaves_name;
    std::string file_delete_leaves_name;
    Compare cpy;
    sjtu::vector<int> delete_;
    int delete_num = 0;
    sjtu::vector<int> delete_leaves;
    int delete_leaves_num = 0;

public:

    explicit bpt(std::string file_name_, std::string file_leaves_name_, std::string file_delete_name_,
                    std::string file_delete_leaves_) ;

    ~bpt() ;

    void insert(const value_type &value) ;

    void insert(const Key &key, const T &value) ;

    sjtu::pair<bool, T> find2(const Key &key) ;

    T find(const Key &key) ;

    void erase(const Key &key) ;

    void modify(const Key &key, const T &v) ;

    bool count(const Key &key) ;

    bool empty() ;

    void clean();

    sjtu::vector<value_type> traverse_val(const Key &l, const Key &r) ;

    sjtu::vector<T> traverse(const Key &l, const Key &r) ;

};
```
