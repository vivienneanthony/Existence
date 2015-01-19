#ifndef ACCOUNT_H_INCLUDED
#define ACCOUNT_H_INCLUDED

#include <iostream>
#include <string>

#define ACCOUNTFILE "account.xml"

using namespace std;

// define useraccount
struct accountinformation
{
    int idx;
    long int uniqueid;
    std::string username;
    std::string email;
    std::string firstname;
    std::string middlename;
    std::string lastname;
    std::string password;
};

#endif // ACCOUNT_H_INCLUDED
