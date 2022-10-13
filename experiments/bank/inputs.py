import argparse
import random
from random import randrange

parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('count', metavar='N', type=int,
                    help='the number of input lines to produce')
parser.add_argument('seed', metavar='S', type=int,
                    help='the random seed',
					default=1337,nargs='?')

args = parser.parse_args()

random.seed(args.seed)

class BaseAction:
	def Logout(self):
		self.logout=True

class Pwd(BaseAction):
	def __init__(self, user, pwd, acctnos, logout):
		self.logout=logout
		self.user=user
		self.pwd=pwd
		self.acctnos=acctnos
	def Next(self):
		print(self.pwd)
		if(self.logout):
			return UserMenuLogout(True)
		else:
			return random.choice([SelectAccount(self.acctnos,self.logout) for x in range(1,20)] + [UserMenuLogout(self.logout)])

class AccountMenuDepositAmount(BaseAction):
	def __init__(self, acctno, acctnos, logout):
		self.logout=logout
		self.acctnos=acctnos
		self.acctno=acctno
	def Next(self):
		print(randrange(1,100))
		return AccountMenuChoice(self.acctno, self.acctnos,self.logout)

class AccountMenuDeposit(BaseAction):
	def __init__(self, acctno, acctnos, logout):
		self.logout=logout
		self.acctnos=acctnos
		self.acctno=acctno
	def Next(self):
		print("1")
		return AccountMenuDepositAmount(self.acctno, self.acctnos, self.logout)

class AccountMenuWithdrawAmount(BaseAction):
	def __init__(self, acctno, acctnos, logout):
		self.logout=logout
		self.acctnos=acctnos
		self.acctno=acctno
	def Next(self):
		print(randrange(1,100))
		return AccountMenuChoice(self.acctno, self.acctnos, self.logout)

class AccountMenuWithdraw(BaseAction):
	def __init__(self, acctno, acctnos, logout):
		self.logout=logout
		self.acctnos=acctnos
		self.acctno=acctno
	def Next(self):
		print("2")
		return AccountMenuWithdrawAmount(self.acctno, self.acctnos, self.logout)

class AccountMenuCheckBalanceReturn(BaseAction):
	def __init__(self, acctno, acctnos, logout):
		self.logout=logout
		self.acctnos=acctnos
		self.acctno=acctno
	def Next(self):
		print("")
		return AccountMenuChoice(self.acctno, self.acctnos, self.logout)

class AccountMenuCheckBalance(BaseAction):
	def __init__(self, acctno, acctnos, logout):
		self.logout=logout
		self.acctnos=acctnos
		self.acctno=acctno
	def Next(self):
		print("3")
		return AccountMenuCheckBalanceReturn(self.acctno, self.acctnos, self.logout)

class AccountMenuTransfer(BaseAction):
	def __init__(self, acctno, acctnos, logout):
		self.logout=logout
		self.acctnos=acctnos
		self.acctno=acctno
	def Next(self):
		print("4")
		return AccountMenuTransferTarget(self.acctno, self.acctnos, self.logout)

class AccountMenuTransferTarget(BaseAction):
	def __init__(self, acctno, acctnos, logout):
		self.logout=logout
		self.acctnos=acctnos
		self.acctno=acctno
	def Next(self):
		recaccount=random.choice([acctno for acctno in self.acctnos if acctno!=self.acctno])
		print(recaccount)
		return AccountMenuTransferAmount(self.acctno, self.acctnos, recaccount, self.logout)

class AccountMenuTransferAmount(BaseAction):
	def __init__(self, acctno, acctnos, target, logout):
		self.logout=logout
		self.acctnos=acctnos
		self.acctno=acctno
		self.target=target
	def Next(self):
		print(randrange(1,100))
		return AccountMenuChoice(self.acctno, self.acctnos, self.logout)

class SelectDifferentAccountNoAction(BaseAction):
	def __init__(self, acctno, acctnos, logout):
		self.logout=logout
		self.acctnos=acctnos
		self.acctno=acctno
	def Next(self):
		print("")
		return AccountMenuChoice(self.acctno, self.acctnos, self.logout)

class AccountMenuSelectDifferentAccount(BaseAction):
	def __init__(self, acctno, acctnos, logout):
		self.logout=logout
		self.acctnos=acctnos
		self.acctno=acctno
	def Next(self):
		print("5")
		if(self.logout):
			return SelectAccountLogout(self.logout)
		else:
			return random.choice([AccountChoice(acctno, self.acctnos, self.logout) for acctno in self.acctnos] + [SelectDifferentAccountNoAction(self.acctno,self.acctnos, self.logout), SelectAccountLogout(self.logout)])

class AccountMenuLogout(BaseAction):
	def __init__(self, logout):
		self.logout=logout
	def Next(self):
		print("0")
		if(self.logout):
			return None
		return LoggedOut(self.logout)

class AccountMenuChoice(BaseAction):
	def __init__(self, acctno, acctnos, logout):
		self.logout=logout
		self.acctnos=acctnos
		self.acctno=acctno
	def Next(self):
		if(self.logout):
			return AccountMenuLogout(self.logout)
		else:
			return random.choice([AccountMenuDeposit(self.acctno,self.acctnos, self.logout), AccountMenuWithdraw(self.acctno,self.acctnos, self.logout), AccountMenuCheckBalance(self.acctno,self.acctnos, self.logout), AccountMenuTransfer(self.acctno,self.acctnos, self.logout), AccountMenuSelectDifferentAccount(self.acctno,self.acctnos, self.logout), AccountMenuLogout(self.logout)]).Next()

class AccountChoice(BaseAction):
	def __init__(self, acctno, acctnos, logout):
		self.logout=logout
		self.acctnos=acctnos
		self.acctno=acctno
	def Next(self):
		print(self.acctno)
		return AccountMenuChoice(self.acctno,self.acctnos, self.logout)

class SelectAccount(BaseAction):
	def __init__(self, acctnos, logout):
		self.logout=logout
		self.acctnos=acctnos
	def Next(self):
		print("1")
		if(self.logout):
			return SelectAccountLogout(self.logout)
		else:
			return random.choice([AccountChoice(acctno, self.acctnos, self.logout) for acctno in self.acctnos] + [SelectAccountNoAction(self.acctnos, self.logout), SelectAccountLogout(self.logout)])

class UserMenuLogout(BaseAction):
	def __init__(self,logout):
		self.logout=logout
	def Next(self):
		print("0")
		if(self.logout):
			return None
		else:
			return LoggedOut(self.logout)

class SelectAccountLogout(BaseAction):
	def __init__(self,logout):
		self.logout=logout
	def Next(self):
		print("L")
		if(self.logout):
			return None
		else:
			return LoggedOut(self.logout)

class SelectAccountNoAction(BaseAction):
	def __init__(self, acctnos, logout):
		self.logout=logout
		self.acctnos=acctnos
	def Next(self):
		print("")
		if(self.logout):
			return UserMenuLogout(self.logout)
		else:
			return random.choice([SelectAccount(self.acctnos, self.logout) for x in range(1,20)] + [UserMenuLogout(self.logout)])

class LoggedOut(BaseAction):
	def __init__(self, logout):
		self.logout=logout
	def Next(self):
		user = random.choice([("user1","pwd1",["100001","100002","100003"]),("user2","pwd2",["100004","100003"])])
		print(user[0])
		return Pwd(user[0],user[1],user[2], self.logout)


state=LoggedOut(False)
for i in range(0,args.count):
	state=state.Next()
state.Logout()
while(state!=None):
	state=state.Next()