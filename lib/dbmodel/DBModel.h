/*********************************************************************
* Copyright (C) 2020 Miguel Revilla Rodríguez
*                    and the OJudge Platform project contributors
*
* This file is part of the OJudge Platform
*
* Read the LICENSE file for information on license terms
*********************************************************************/

#ifndef DBMODEL_H
#define DBMODEL_H

#include <Wt/Auth/Login.h>
#include <Wt/Auth/AuthService.h>
#include <Wt/Auth/PasswordService.h>
#include <Wt/Auth/GoogleService.h>
#include <Wt/Auth/FacebookService.h>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/Dbo/Types.h>
#include <Wt/WDate.h>
#include <Wt/WGlobal.h>
#include <Wt/Auth/Dbo/AuthInfo.h>
#include <string>

using namespace Wt;

namespace dbo = Wt::Dbo;

namespace {
Auth::AuthService myAuthService;
Auth::PasswordService myPasswordService(myAuthService);
std::vector<std::unique_ptr<Auth::OAuthService> > myOAuthServices;
}

enum class Role {
	Visitor = 0,
	Registered = 1,
	Moderator = 2,
	Publisher = 3,
	Editor = 4,
	Admin = 32
};

enum class AvatarType {
	Default = 0,
	Gravatar = 1,
	Custom = 2
};

class User;
class UserSettings;
class UserAvatar;
class Category;
class Problem;
class Description;
class Setting;
class Submission;
class Testcase;
class Verdict;
class Language;
class Contest;

typedef Wt::Auth::Dbo::AuthInfo<User> AuthInfo;
typedef Auth::Dbo::UserDatabase<AuthInfo> UserDatabase;

typedef dbo::collection< dbo::ptr<User> > Users;
typedef dbo::collection< dbo::ptr<Category> > Categories;
typedef dbo::collection< dbo::ptr<Problem> > Problems;
typedef dbo::collection< dbo::ptr<Description> > Descriptions;
typedef dbo::collection< dbo::ptr<Testcase> > Testcases;
typedef dbo::collection< dbo::ptr<Submission> > Submissions;
typedef dbo::collection< dbo::ptr<Verdict> > Verdicts;
typedef dbo::collection< dbo::ptr<Setting> > Settings;
typedef dbo::collection< dbo::ptr<Language> > Languages;
typedef dbo::collection< dbo::ptr<Contest> > Contests;

namespace Wt {
namespace Dbo {
template<>
struct dbo_traits<Problem> : public dbo_default_traits {
	static const char *surrogateIdField() {
		return 0;
	}
};


template<>
struct dbo_traits<Description> : public dbo_default_traits {
	typedef ptr<Problem> IdType;

	static IdType invalidId() {
		return ptr<Problem>{};
	}
	static const char *surrogateIdField() {
		return 0;
	}
};

template<>
struct dbo_traits<UserSettings> : public dbo_default_traits {
	typedef ptr<User> IdType;

	static IdType invalidId() {
		return ptr<User>{};
	}
	static const char *surrogateIdField() {
		return 0;
	}
};

template<>
struct dbo_traits<UserAvatar> : public dbo_default_traits {
	typedef ptr<User> IdType;

	static IdType invalidId() {
		return ptr<User>{};
	}
	static const char *surrogateIdField() {
		return 0;
	}
};

}
}

class Session : public dbo::Session {
public:
static void configureAuth();

Session(const std::string &postgresDb);
~Session();

void loadUsers();

dbo::ptr<User> user();
dbo::ptr<User> user(const Auth::User &user);

Auth::AbstractUserDatabase &users();
Auth::Login &login() {
	return login_;
}

void createUserData(const Auth::User &newUser);

static const Auth::AuthService &auth();
static const Auth::PasswordService &passwordAuth();
static const std::vector<const Auth::OAuthService*> oAuth();

private:
std::unique_ptr<UserDatabase> users_;
Auth::Login login_;
};

class User {
public:
dbo::weak_ptr<AuthInfo> authInfo;
Role role;
std::optional<std::string> firstName;
std::optional<std::string> lastName;
std::optional<Wt::WDate> birthday;
std::optional<std::string> institution;
std::optional<std::string> country;
dbo::weak_ptr<UserAvatar> avatar;
std::optional<std::string> uvaID;
std::optional<bool> firstlogin;
Submissions submissions;
dbo::weak_ptr<UserSettings> settings;

const std::string avatarLink(const AvatarType type, int size = 128) const;

template<class Action>
void persist(Action& a)
{
	dbo::hasOne(a, authInfo, "user");
	dbo::field(a, role, "role");
	dbo::field(a, firstName, "firstname");
	dbo::field(a, lastName, "lastname");
	dbo::field(a, birthday, "birthday");
	dbo::field(a, institution, "institution");
	dbo::field(a, country, "country");
	dbo::hasOne(a, avatar, "user");
	dbo::field(a, uvaID, "uvaid");
	dbo::field(a, firstlogin, "firstlogin");
	dbo::hasMany(a, submissions, dbo::ManyToOne, "user");
	dbo::hasOne(a, settings, "user");
}
};

class UserSettings {
public:
dbo::ptr<User> user;
std::optional<int> editor_fontsize;
std::optional<int> editor_indent;
std::optional<bool> editor_wrap;
std::optional<std::string> editor_theme;
std::optional<bool> notifications_email_results;
std::optional<bool> notifications_email_contests;
std::optional<bool> notifications_email_general;
std::optional<bool> notifications_browser_results;
std::optional<bool> notifications_browser_contests;
std::optional<bool> notifications_browser_general;
std::optional<bool> twofa_enabled;
std::optional<std::vector<unsigned char> > twofa_secret;

template<class Action>
void persist(Action& a) {
	dbo::id(a, user, "user", dbo::NotNull|dbo::OnDeleteCascade);
	dbo::field(a, editor_fontsize, "editor_fontsize");
	dbo::field(a, editor_indent, "editor_indent");
	dbo::field(a, editor_wrap, "editor_wrap");
	dbo::field(a, editor_theme, "editor_theme");
	dbo::field(a, notifications_email_results, "notifications_email_results");
	dbo::field(a, notifications_email_contests, "notifications_email_contests");
	dbo::field(a, notifications_email_general, "notifications_email_general");
	dbo::field(a, notifications_browser_results, "notifications_browser_results");
	dbo::field(a, notifications_browser_contests, "notifications_browser_contests");
	dbo::field(a, notifications_browser_general, "notifications_browser_general");
	dbo::field(a, twofa_enabled, "twofa_enabled");
	dbo::field(a, twofa_secret, "twofa_secret");
}
};

class UserAvatar {
public:
dbo::ptr<User> user;
AvatarType avatarType;
std::optional<std::vector<unsigned char> > avatar;

template<class Action>
void persist(Action& a) {
	dbo::id(a, user, "user", dbo::NotNull|dbo::OnDeleteCascade);
	dbo::field(a, avatarType, "avatartype");
	dbo::field(a, avatar, "avatar");
}
};

class Category {
public:
std::string title;
int order;

dbo::ptr<Category> parent;
Categories children;
Problems problems;

template<class Action>
void persist(Action& a) {
	dbo::field(a, title, "title");
	dbo::field(a, order, "order");
	dbo::belongsTo(a, parent, "parent");
	dbo::hasMany(a, children, dbo::ManyToOne, "parent");
	dbo::hasMany(a, problems, dbo::ManyToMany, "probs_cats");
}
};

class Problem {
public:
long long id;
std::string title;

Categories categories;
Testcases testcases;
Submissions submissions;
dbo::weak_ptr<Description> description;
Contests contests;

template<class Action>
void persist(Action& a) {
	dbo::id(a, id, "id");
	dbo::field(a, title, "title");
	dbo::hasOne(a, description);
	dbo::hasMany(a, categories, dbo::ManyToMany, "probs_cats");
	dbo::hasMany(a, testcases, dbo::ManyToOne, "problem");
	dbo::hasMany(a, submissions, dbo::ManyToOne, "problem");
	dbo::hasMany(a, contests, dbo::ManyToMany, "probs_contests");
}
};

class Description {
public:
std::optional< std::string > htmldata;
std::optional< std::vector<unsigned char> > pdfdata;
dbo::ptr<Problem> problem;

template<class Action>
void persist(Action& a) {
	dbo::id(a, problem, "problem", dbo::NotNull|dbo::OnDeleteCascade);
	dbo::field(a, htmldata, "htmldata");
	dbo::field(a, pdfdata, "pdfdata");
}
};

class Language {
public:
std::string name;
std::string compilerVersion;
std::optional< std::vector<unsigned char> > codeSkeleton;
std::optional< std::string > aceStyle;
std::optional< std::vector<unsigned char> > compileScript;
std::optional< std::vector<unsigned char> > linkScript;
std::vector<unsigned char> runScript;
Submissions submissions;
Contests contests;

template<class Action>
void persist(Action& a) {
	dbo::field(a, name, "name");
	dbo::field(a, compilerVersion, "compilerversion");
	dbo::field(a, codeSkeleton, "codeskeleton");
	dbo::field(a, aceStyle, "acestyle");
	dbo::field(a, compileScript, "compilescript");
	dbo::field(a, linkScript, "linkscript");
	dbo::field(a, runScript, "runscript");
	dbo::hasMany(a, submissions, dbo::ManyToOne, "language");
	dbo::hasMany(a, contests, dbo::ManyToMany, "langs_contests");
}
};

class Contest {
public:
std::string title;
std::optional<std::string> subTitle;
std::optional< std::vector<unsigned char> > description;
std::optional< std::vector<unsigned char> > logo;
bool isVirtual;
WDateTime startTime;
WDateTime endTime;
Problems problems;
Languages languages;
Submissions submissions;

template<class Action>
void persist(Action& a) {
	dbo::field(a, title, "title");
	dbo::field(a, subTitle, "subtitle");
	dbo::field(a, description, "description");
	dbo::field(a, logo, "logo");
	dbo::field(a, isVirtual, "isvirtual");
	dbo::field(a, startTime, "starttime");
	dbo::field(a, endTime, "endtime");
	dbo::hasMany(a, problems, dbo::ManyToMany, "probs_contests");
	dbo::hasMany(a, languages, dbo::ManyToMany, "langs_contests");
	dbo::hasMany(a, submissions, dbo::ManyToOne, "contest");
}
};

class Testcase {
public:
std::string title;
dbo::ptr<Problem> problem;

template<class Action>
void persist(Action& a) {
	dbo::field(a, title, "title");
	dbo::belongsTo(a, problem, "problem", Dbo::NotNull);
}
};

class Setting {
public:
std::string settingName;
std::string settingValue;

template<class Action>
void persist(Action& a) {
	dbo::field(a, settingName, "settingname");
	dbo::field(a, settingValue, "settingvalue");
}
};

class Submission {
public:
dbo::ptr<Problem> problem;
dbo::ptr<User> user;
dbo::ptr<Testcase> testcase;
dbo::ptr<Contest> contest;
WDateTime datetime;
dbo::ptr<Language> language;
Verdicts verdicts;

template<class Action>
void persist(Action& a) {
	dbo::belongsTo(a, problem, "problem", Dbo::NotNull);
	dbo::belongsTo(a, user, "user", Dbo::NotNull);
	dbo::belongsTo(a, testcase, "testcase", Dbo::NotNull);
	dbo::belongsTo(a, contest, "contest");
	dbo::field(a, datetime, "datetime");
	dbo::belongsTo(a, language, "language", Dbo::NotNull);
	dbo::hasMany(a, verdicts, dbo::ManyToOne, "submission");
}
};

class Verdict {
public:
dbo::ptr<Submission> submission;
WDateTime datetime;
int cpuused;
int memoryused;
std::optional<std::string> reason;

template<class Action>
void persist(Action& a) {
	dbo::belongsTo(a, submission, "submission", Dbo::NotNull);
	dbo::field(a, datetime, "datetime");
	dbo::field(a, cpuused, "cpuused");
	dbo::field(a, memoryused, "memoryused");
	dbo::field(a, reason, "reason");
}
};

class DBModel {
public:
DBModel(Session* session);
~DBModel();

dbo::Transaction startTransaction();

Categories getCategories();
dbo::ptr<Category> addCategory(std::string title, int parent);

Problems getProblems();
dbo::ptr<Problem> getProblem(long long id);
dbo::ptr<Problem> addProblem(long long id, std::string title);
void setProblemCategories(long long id, std::set<int> categories);
void updateDescription(long long problemId, std::optional<std::string> htmlData, std::optional<std::vector<unsigned char> > pdfData);

std::string getSiteSetting(std::string settingName);
void updateSiteSetting(std::string settingName, std::string settingValue);

Languages getLanguages();

private:
Session* session_;
};

#endif // DBMODEL_H
