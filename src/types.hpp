#pragma once

#include <codecvt>
#include <locale>

#include "stdafx.h"

using namespace std;
using jtype = web::json::value::value_type;

static string ws2s(const std::wstring& wstr) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
};

static wstring s2ws(const std::string& str) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
};

class FormatException : public std::exception {
    char message[100];

public:
    FormatException(const wstring field, const wstring type) {
        std::wstring t(field);
        t.append(L" should be ");
        t.append(type);

        size_t i;
        wcstombs_s(&i, message, t.length() + 1, t.c_str(), t.length());
    }
    const char* what() const throw() {
        return message;
    }
};

class ManagerException : public std::exception {
    char description[100];

public:
    int code;
    ManagerException(const int code_, const char* description_) {
        code = code_;
        strncpy_s(description, description_, 100);
    }

    const char* what() const throw() {
        return description;
    }
};

class RequiredException : public std::exception {
    char message[100];

public:
    RequiredException(const std::wstring field) {
        std::wstring t(field);
        t.append(L" is required");

        size_t i;
        wcstombs_s(&i, message, t.length() + 1, t.c_str(), t.length());
    }
    const char* what() const throw() {
        return message;
    }
};

struct Balance {
    double balance;

    web::json::value AsJSON() const {
        web::json::value result = web::json::value::object();

        result[U("balance")] = web::json::value::number(balance);

        return result;
    }
};

struct JMarginLevel {
    utility::string_t group;
    int leverage = 0;
    int volume = 0;
    double balance = 0;
    double equity = 0;
    double margin = 0;
    double margin_free = 0;
    double margin_level = 0;
    int margin_type = 0;
    int level_type = 0;
    double last_month_balance = 0;
    double last_day_balance = 0;

    static JMarginLevel FromMarginLevel(const MarginLevel level, const UserRecord u) {
        JMarginLevel l;

        std::wstring wgroup(level.group, level.group + strlen(level.group));
        l.group = wgroup;

        l.balance = level.balance;
        l.leverage = level.leverage;
        l.volume = level.volume;
        l.equity = level.equity;
        l.margin = level.margin;
        l.margin_free = level.margin_free;
        l.margin_level = level.margin_level;
        l.margin_type = level.margin_type;
        l.level_type = level.level_type;
        l.last_month_balance = u.prevmonthbalance;
        l.last_day_balance = u.prevbalance;

        return l;
    }



    web::json::value AsJSON() const {
        web::json::value result = web::json::value::object();

        result[U("group")] = web::json::value::string(group);
        result[U("balance")] = web::json::value::number(balance);
        //result[U("leverage")] = web::json::value::number(leverage);
       // result[U("volume")] = web::json::value::number(volume);
        result[U("equity")] = web::json::value::number(equity);
        //  result[U("margin")] = web::json::value::number(margin);
        //  result[U("margin_free")] = web::json::value::number(margin_free);
          //result[U("margin_level")] = web::json::value::number(margin_level);
         // result[U("margin_type")] = web::json::value::number(margin_type);
         // result[U("level_type")] = web::json::value::number(level_type);
        result[U("last_day_balance")] = web::json::value::number(last_day_balance);
        result[U("last_month_balance")] = web::json::value::number(last_month_balance);


        return result;
    }
};

struct JUser {
    utility::string_t group;
    utility::string_t name;
    utility::string_t email;
    utility::string_t city;
    utility::string_t state;
    utility::string_t country;
    utility::string_t address;
    utility::string_t comment;
    utility::string_t status;
    utility::string_t phone;
    utility::string_t phone_password;
    utility::string_t lead_source;
    utility::string_t zipcode;

    int login;
    int agent_account;
    int id_number;
    int leverage;
    int enabled;
    int enable_change_password;
    int send_reports;
    int read_only;
    time_t reg_date;

    double balance, credit, last_day_balance, last_month_balance, last_day_equity, last_month_equity;

    JMarginLevel jLevel;

    static JUser FromUserRecord(const UserRecord& user) {
        JUser u;
        wstring wname(user.name, user.name + strlen(user.name));
        u.name = wname;

        wstring wgroup(user.group, user.group + strlen(user.group));
        u.group = wgroup;

        wstring wemail(user.email, user.email + strlen(user.email));
        u.email = wemail;

        wstring wcity(user.city, user.city + strlen(user.city));
        u.city = wcity;

        wstring wstate(user.state, user.state + strlen(user.state));
        u.state = wstate;

        wstring waddress(user.address, user.address + strlen(user.address));
        u.address = waddress;

        wstring wstatus(user.status, user.status + strlen(user.status));
        u.status = wstatus;

        wstring wcomment(user.comment, user.comment + strlen(user.comment));
        u.comment = wcomment;

        wstring wphone(user.phone, user.phone + strlen(user.phone));
        u.phone = wphone;

        wstring wlead(user.lead_source, user.lead_source + strlen(user.lead_source));
        u.lead_source = wlead;

        wstring wzipcode(user.zipcode, user.zipcode + strlen(user.zipcode));
        u.zipcode = wzipcode;

        wstring wphone_password(user.password_phone, user.password_phone + strlen(user.password_phone));
        u.phone_password = wphone_password;

        u.login = user.login;
        u.balance = user.balance;
        u.credit = user.credit;
        u.enabled = user.enable;
        u.leverage = user.leverage;
        u.agent_account = user.agent_account;
        u.send_reports = user.send_reports;
        u.read_only = user.enable_read_only;
        u.id_number = user.mqid;
        u.enable_change_password = user.enable_change_password;
        u.reg_date = user.regdate;
        u.last_day_balance = user.prevbalance;
        u.last_month_balance = user.prevmonthbalance;
        u.last_day_equity = user.prevequity;
        u.last_month_equity = user.prevmonthequity;

        return u;
    }

    void FillMarginLevel(const MarginLevel& level, const UserRecord u) {
        jLevel = JMarginLevel::FromMarginLevel(level, u);
    }

    web::json::value AsJSON() const {
        web::json::value result = web::json::value::object();

        result[U("login")] = web::json::value::number(login);
        result[U("name")] = web::json::value::string(name);
        result[U("group")] = web::json::value::string(group);
        result[U("email")] = web::json::value::string(email);
        result[U("city")] = web::json::value::string(city);
        result[U("state")] = web::json::value::string(state);
        result[U("country")] = web::json::value::string(country);
        result[U("address")] = web::json::value::string(address);
        result[U("comment")] = web::json::value::string(comment);
        result[U("status")] = web::json::value::string(status);
        result[U("phone")] = web::json::value::string(phone);
        result[U("lead_source")] = web::json::value::string(lead_source);
        result[U("phone_password")] = web::json::value::string(phone_password);
        result[U("agent_account")] = web::json::value::number(agent_account);
        result[U("zipcode")] = web::json::value::string(zipcode);

        result[U("id_number")] = web::json::value::number(id_number);
        result[U("leverage")] = web::json::value::number(leverage);
        result[U("enabled")] = web::json::value::number(enabled);
        result[U("enable_change_password")] = web::json::value::number(enable_change_password);
        result[U("send_reports")] = web::json::value::number(send_reports);
        result[U("read_only")] = web::json::value::number(read_only);
        result[U("reg_date")] = web::json::value::number(reg_date);
        result[U("last_day_balance")] = web::json::value::number(last_day_balance);
        result[U("last_month_balance")] = web::json::value::number(last_month_balance);
        result[U("last_day_equity")] = web::json::value::number(last_day_equity);
        result[U("last_month_equity")] = web::json::value::number(last_month_equity);

        wchar_t buffer[32];
        struct std::tm ptm;
        _putenv("TZ=Asia/Singapore");
        localtime_s(&ptm, &reg_date);

        std::wcsftime(buffer, 32, U("%Y.%m.%d %H:%M:%S"), &ptm);
        result[U("reg_date_str")] = web::json::value::string(buffer);

        result[U("margin")] = jLevel.AsJSON();

        return result;
    }
};

struct UserResponse {
    utility::string_t email;
    int login;
    int error_code = 0;
    char error_description[100];

    static UserResponse FromUserRecord(const UserRecord& user, const int code_, const char* description_) {
        UserResponse u;

        wstring wemail(user.email, user.email + strlen(user.email));
        u.email = wemail;

        u.login = user.login;
        u.error_code = code_;
        strncpy_s(u.error_description, description_, 100);

        return u;
    }

    static UserResponse FromError(const int code_, const char* description_) {
        UserResponse u;

        u.error_code = code_;
        strncpy_s(u.error_description, description_, 100);

        return u;
    }

    web::json::value AsJSON() const {
        web::json::value result = web::json::value::object();

        result[U("login")] = web::json::value::number(login);
        result[U("email")] = web::json::value::string(email);
        result[U("error")] = web::json::value::number(error_code);

        wstring t(error_description, error_description + strlen(error_description));
        result[U("message")] = web::json::value::string(t);

        return result;
    }
};

struct jTick {
    utility::string_t name;
    double bid, ask;

    web::json::value AsJSON() const {
        web::json::value result = web::json::value::object();

        result[U("name")] = web::json::value::string(name);
        result[U("bid")] = web::json::value::number(bid);
        result[U("ask")] = web::json::value::number(ask);

        return result;
    }
};

struct jSymbol {
    utility::string_t name;
    double spread;
    int symgroup_index;

    static jSymbol FromConSymbol(const ConSymbol& con) {
        jSymbol symbol;

        wstring sym(con.symbol, con.symbol + strlen(con.symbol));
        symbol.name = sym;
        symbol.spread = con.spread;
        symbol.symgroup_index = con.type;

        return symbol;
    }

    web::json::value AsJSON() const {
        web::json::value result = web::json::value::object();

        result[U("name")] = web::json::value::string(name);
        result[U("spread")] = web::json::value::number(spread);
        result[U("symgroup_index")] = web::json::value::number(symgroup_index);

        return result;
    }
};

struct jJournalRecord {
    utility::string_t message;
    utility::string_t ip;
    utility::string_t time;
    int code;

    static jJournalRecord FromServerLog(const ServerLog& record) {
        jJournalRecord jRecord;

        wstring msg(record.message, record.message + strlen(record.message));
        jRecord.message = msg;

        wstring t(record.time, record.time + strlen(record.time));
        jRecord.time = t;

        wstring p(record.ip, record.ip + strlen(record.ip));
        jRecord.ip = p;

        jRecord.code = record.code;

        return jRecord;
    }

    web::json::value AsJSON() const {
        web::json::value result = web::json::value::object();

        result[U("message")] = web::json::value::string(message);
        result[U("time")] = web::json::value::string(time);
        result[U("ip")] = web::json::value::string(ip);
        result[U("code")] = web::json::value::number(code);

        return result;
    }
};

struct jMargin {
    double equity, balance, last_month_balance;

    static jMargin FromMarginLevel(const MarginLevel& lvl) {
        jMargin obj;

        obj.equity = lvl.equity;
        obj.balance = lvl.balance;
        obj.last_month_balance = 99;

        return obj;
    }

    web::json::value AsJSON() const {
        web::json::value result = web::json::value::object();

        result[U("equity")] = web::json::value::number(equity);
        result[U("balance")] = web::json::value::number(balance);
        result[U("last_month_balance")] = web::json::value::number(last_month_balance);

        return result;
    }
};

struct jUserOnline {
    int login;                // user login
    UINT ip;                  // connection ip address
    utility::string_t group;  // user group

    static jUserOnline FromOnlineRecord(const OnlineRecord& record) {
        jUserOnline obj;

        obj.login = record.login;
        obj.ip = record.ip;

        wstring wgroup(record.group, record.group + strlen(record.group));
        obj.group = wgroup;

        return obj;
    }

    web::json::value AsJSON() const {
        web::json::value result = web::json::value::object();

        result[U("login")] = web::json::value::number(login);
        result[U("ip")] = web::json::value::number(ip);
        result[U("group")] = web::json::value::string(group);

        return result;
    }
};

struct JGroupSecurity {
    int trade;            // enable show and trade for this group of securites
    int execution;        // dealing mode-EXECUTION_MANUAL,EXECUTION_AUTO,EXECUTION_ACTIVITY
    double comm_base;     // standart commission
    int comm_type;        // commission type-COMM_TYPE_MONEY,COMM_TYPE_PIPS,COMM_TYPE_PERCENT
    int comm_lots;        // commission lots mode-COMMISSION_PER_LOT,COMMISSION_PER_DEAL
    double comm_agent;    // agent commission
    int comm_agent_type;  // agent commission mode-COMM_TYPE_MONEY, COMM_TYPE_PIPS
    //---
    int spread_diff;  // spread difference in compare with default security spread
    //---
    int lot_min, lot_max;  // allowed minimal and maximal lot values
    int lot_step;          // allowed step value (10 lot-1000, 1 lot-100, 0.1 lot-10)

    static JGroupSecurity FromConGroupSec(const ConGroupSec& sec) {
        JGroupSecurity s;

        s.trade = sec.trade;
        s.execution = sec.execution;
        s.comm_base = sec.comm_base;
        s.comm_type = sec.comm_type;
        s.comm_lots = sec.comm_lots;
        s.comm_agent = sec.comm_agent;
        s.comm_agent_type = sec.comm_agent_type;

        s.spread_diff = sec.spread_diff;
        s.lot_min = sec.lot_min;
        s.lot_max = sec.lot_max;
        s.lot_step = sec.lot_step;

        return s;
    }

    web::json::value AsJSON() const {
        web::json::value result = web::json::value::object();

        result[U("trade")] = web::json::value::number(trade);
        result[U("execution")] = web::json::value::number(execution);
        result[U("comm_base")] = web::json::value::number(comm_base);
        result[U("comm_type")] = web::json::value::number(comm_type);
        result[U("comm_lots")] = web::json::value::number(comm_lots);
        result[U("comm_agent")] = web::json::value::number(comm_agent);
        result[U("comm_agent_type")] = web::json::value::number(comm_agent_type);

        result[U("spread_diff")] = web::json::value::number(spread_diff);
        result[U("lot_min")] = web::json::value::number(lot_min);
        result[U("lot_max")] = web::json::value::number(lot_max);
        result[U("lot_step")] = web::json::value::number(lot_step);

        return result;
    }
};

struct JGroup {
    utility::string_t name, currency;
    web::json::value securities;

    int default_leverage;

    static JGroup FromGroupRecord(const ConGroup& group) {
        JGroup g;

        wstring wname(group.group, group.group + strlen(group.group));
        g.name = wname;

        wstring wcurrency(group.currency, group.currency + strlen(group.currency));
        g.currency = wcurrency;

        g.default_leverage = group.default_leverage;

        vector<JGroupSecurity> v;

        for (int j = 0; j < MAX_SEC_GROUPS; j++) {
            if (group.secgroups[j].show == 1) {
                JGroupSecurity gs = JGroupSecurity::FromConGroupSec(group.secgroups[j]);
                v.push_back(gs);
            }
        }

        g.securities = web::json::value::array(v.size());
        int idx = 0;
        for (auto iter = v.begin(); iter != v.end(); iter++) {
            g.securities[idx++] = iter->AsJSON();
        }

        return g;
    }

    web::json::value AsJSON() const {
        web::json::value result = web::json::value::object();

        result[U("name")] = web::json::value::string(name);
        result[U("currency")] = web::json::value::string(currency);
        result[U("default_leverage")] = web::json::value::number(default_leverage);

        result[U("securities")] = securities;

        return result;
    }
};

struct jTrade {
    int ticket, login, volume, type, state, digits;
    utility::string_t symbol;
    char reason;

    double profit, close_price, open_price, swap, commission, sl, tp, pips, lots;
    time_t open_time, close_time;

    static jTrade FromTradeTransInfo(TradeTransInfo info) {
        jTrade _trade;

        _trade.ticket = info.order;
        _trade.login = info.orderby;
        _trade.type = info.cmd;
        _trade.sl = info.sl;
        _trade.tp = info.tp;
        wstring sym(info.symbol, info.symbol + strlen(info.symbol));
        _trade.symbol = sym;
        _trade.close_price = 0;
        _trade.open_price = info.price;
        _trade.volume = info.volume;
        _trade.close_time = 0;

        return _trade;
    }

    static jTrade FromTradeRecord(const TradeRecord& trade) {
        jTrade _trade;
        int d;

        d = trade.digits;

        if (trade.digits == 3 || trade.digits == 5)
            d--;

        _trade.ticket = trade.order;
        _trade.login = trade.login;
        _trade.type = trade.cmd;
        _trade.commission = trade.commission;
        _trade.profit = trade.profit;
        _trade.state = trade.state;
        _trade.digits = d;

        // todo::calc pips
        if (trade.cmd == 0) {
            _trade.pips = (trade.close_price - trade.open_price) * pow(10, d);
        }
        else {
            _trade.pips = (trade.open_price - trade.close_price) * pow(10, d);
        }

        _trade.sl = trade.sl;
        _trade.tp = trade.tp;
        wstring sym(trade.symbol, trade.symbol + strlen(trade.symbol));
        _trade.symbol = sym;

        _trade.reason = trade.reason;

        _trade.close_price = trade.close_price;
        _trade.open_price = trade.open_price;
        _trade.volume = trade.volume;
        _trade.lots = trade.volume / 10000;
        _trade.close_time = trade.close_time;
        _trade.open_time = trade.open_time;

        return _trade;
    }

    web::json::value AsJSON() const {
        web::json::value result = web::json::value::object();
        utility::string_t sType;

        switch (type) {
        case 0:
            sType = U("buy");
            break;
        case 1:
            sType = U("sell");
            break;
        case 2:
            sType = U("buy limit");
            break;
        case 3:
            sType = U("sell limit");
            break;
        case 4:
            sType = U("buy stop");
            break;
        case 5:
            sType = U("sell stop");
            break;
        case 6:
            sType = U("balance");
            break;
        case 7:
            sType = U("credit");
            break;
        }

        result[U("reason")] = web::json::value::number(reason);
        result[U("symbol")] = web::json::value::string(symbol);
        result[U("state")] = web::json::value::number(state);
        result[U("type")] = web::json::value::number(type);
        result[U("type_str")] = web::json::value::string(sType);
        result[U("ticket")] = web::json::value::number(ticket);
        result[U("login")] = web::json::value::number(login);
        result[U("profit")] = web::json::value::number(profit);
        result[U("digits")] = web::json::value::number(digits);
        result[U("sl")] = web::json::value::number(sl);
        result[U("tp")] = web::json::value::number(tp);
        result[U("pips")] = web::json::value::number(pips);
        result[U("volume")] = web::json::value::number(volume);
        result[U("lots")] = web::json::value::number(lots);
        result[U("swap")] = web::json::value::number(swap);
        result[U("commission")] = web::json::value::number(commission);
        result[U("open_price")] = web::json::value::number(open_price);
        result[U("close_price")] = web::json::value::number(close_price);
        result[U("open_time")] = web::json::value::number(open_time);
        result[U("close_time")] = web::json::value::number(close_time);

        wchar_t buffer[32];
        struct std::tm ptm;
        localtime_s(&ptm, &open_time);

        std::wcsftime(buffer, 32, U("%Y.%m.%d %H:%M:%S"), &ptm);
        result[U("open_time_str")] = web::json::value::string(buffer);

        localtime_s(&ptm, &close_time);
        std::wcsftime(buffer, 32, U("%Y.%m.%d %H:%M:%S"), &ptm);
        result[U("close_time_str")] = web::json::value::string(buffer);

        return result;
    }
};

struct Error {
    utility::string_t description;
    int code;

    web::json::value AsJSON() const {
        web::json::value result = web::json::value::object();

        result[U("descr")] = web::json::value::string(description);
        result[U("code")] = web::json::value::number(code);

        return result;
    }
};

struct People {
    utility::string_t name;
    double age;

    static People FromJSON(const web::json::object& object) {
        People result;
        result.name = object.at(U("name")).as_string();
        result.age = object.at(U("age")).as_integer();
        return result;
    }

    web::json::value AsJSON() const {
        web::json::value result = web::json::value::object();
        result[U("name")] = web::json::value::string(name);
        result[U("age")] = web::json::value::number(age);
        return result;
    }
};

struct Data {
    std::vector<People> peoples;
    utility::string_t job;

    Data() {}

    void Clear() { peoples.clear(); }

    static Data FromJSON(const web::json::object& object) {
        Data res;

        web::json::value cs = object.at(U("data"));

        for (auto iter = cs.as_array().begin(); iter != cs.as_array().end(); ++iter) {
            if (!iter->is_null()) {
                People people;
                people = People::FromJSON(iter->as_object());
                res.peoples.push_back(people);
            }
        }

        auto job = object.find(U("job"));
        res.job = job->second.as_string();
        return res;
    }

    web::json::value AsJSON() const {
        web::json::value res = web::json::value::object();
        res[U("job")] = web::json::value::string(job);

        web::json::value jPeoples = web::json::value::array(peoples.size());

        int idx = 0;
        for (auto iter = peoples.begin(); iter != peoples.end(); iter++) {
            jPeoples[idx++] = iter->AsJSON();
        }

        res[U("people")] = jPeoples;
        return res;
    }
};