#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

const int DAY[]={0,1,2,3,4,5,30,31,31,9};
struct Date{
	int m,d,hr,mi;
	
	int change(int fl){
		if(fl==0)return (24-hr)*60-mi;
		return hr*60+mi;
	}
	
	bool Compare(const Date &res){
		return hr<res.hr||hr==res.hr&&mi<=res.mi;
	}
	
	int del(const Date &res){
		return (hr-res.hr)*60+mi-res.mi;
	}
	
	void print(){
		std::cout<<'0'<<m<<'-';
		if(d<10)std::cout<<'0';
		std::cout<<d;
		if(hr<10)std::cout<<" 0";
		else std::cout<<' ';
		std::cout<<hr<<':';
		if(mi<10)std::cout<<'0';
		std::cout<<mi;
	} 
	
	void operator += (const int &t){
		mi+=t;
		hr+=mi/60;
		mi%=60;
		d+=hr/24;
		hr%=24;
		while(DAY[m]<d)d-=DAY[m++];
	}
	
	bool operator < (const Date &res)const{
		return m<res.m|| m==res.m&&d<res.d;
	}
	
	Date():m(0),d(0),hr(0),mi(0){};
	
	Date(const std::string &s){
		m=d=hr=mi=0; 
		if(s[2]=='-')m=s[1]-'0',d=(s[3]-'0')*10+s[4]-'0';
		else hr=(s[0]-'0')*10+s[1]-'0',mi=(s[3]-'0')*10+s[4]-'0';
	}
	
	Date(const int &m_,const int &d_,const int &hr_,const int &mi_):
		m(m_),d(d_),hr(hr_),mi(mi_){}
	
	Date(const Date &res):
		m(res.m),d(res.d),hr(res.hr),mi(res.mi){}
	
	int operator - (const Date &res)const{
		int ret=0;
		if(m==res.m)return d-res.d;
		int D=res.d,M=res.m;
		while(M<m){
			ret+=DAY[M++]-D+1;
			D=1;
		}
		return ret+d-D;
	}
};

/*
-�����������ļ���

(string,��������)�����캯��

+=�ӵ��� ����

<�ݶ�������

*/
