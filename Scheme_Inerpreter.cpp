/* 
+ Implementation of a thorough Interpreter for Scheme Language
+ Design and Implemented by Aslan Mehrabi 

+ Details of the implemented Interpreter:
- Various number types of Scheme [Integer, Rational, Complex, and Polynomial] has been supported
The four basic mathematical operations (+,-,*,/) has been supported
- Functional and Data Driven structure of Scheme language has been conserved
- Specific Data Structures has been developed for utilization of user defined functions of Scheme


==> Text files named "test.scm" (which can be found in the current repository), is available for the test of programm by some sample scripts of Scheme language

*/


//*********************



#include <iostream>
#include <string>
#include <vector>
using namespace std;

//@@@@@@  pass by pointer(Env va ... ) ke zudtar she
//@@@@ delet kardane fazahaye azad shode

/*
(define (eval exp env)
(cond ((self-evaluating? exp) exp)
((variable? exp) (lookup-variable-value exp env))
((quoted? exp) (text-of-quotation exp))
((assignment? exp) (eval-assignment exp env))
((definition? exp) (eval-definition exp env))
((if? exp) (eval-if exp env))
((lambda? exp)
(make-procedure (lambda-parameters exp)
(lambda-body exp)
env))
((begin? exp)
(eval-sequence (begin-actions exp) env))
((cond? exp) (eval (cond->if exp) env))
((application? exp)
(apply (eval (operator exp) env)
(list-of-values (operands exp) env)))
(else
(error "Unknown expression type: EVAL" exp))))
*/
class Exp{
public:
	string type;
	void * next;	// ba tavajoh be noe type, next be classe marbute eshare khahad kard
};


class Environment{
public:

	static string tag;
	vector <string> variables;
	vector <Exp> values;   //@@@@@@@@ dalile entekhabe value tu gozaresh
	Environment * enclosing; // pointer be Environmente pedar

	//empty environment
};

class SelfEvaluating{
public:
	static string tag;
	static string typeStr;
	static string typeNumber;
	string type;	// motanaseb ba type, str ya number meghdar khahand dasht
	string str;
	int number;
};

class Variable{//@@@@@@@@@@@@@@@@@@@@@@@@  khubdan tu tabe va mamuli fargh mikone va VA AGE TABE, SEDA KARDANE MAKE LAMBDA
public:
	static string tag;
	string name;
};


//@@@@@@@@@  ? chi karesh konam?
class Quoted{
public:
	static string tag;
	string name;
};


//struct Qote{
//	int xx;
//	Qote(){
//		xx=QOTE;
//	}
//};

class Assignment{ //set!
public:
	static string tag;
	Variable var;
	Exp value;

};

class Definition{
public:
	static string tag;
	Variable var;
	Exp value;  //@@@@@@@ age tabe bashe=> make lambda
};

class If{
public:
	static string tag;
	Exp predicate;
	Exp consequent;
	Exp alternative;
	bool hasAlternative;

};

class Lambda{   //procedure misaze
public:
	static string tag;
	vector <string> parameters;
	vector <Exp> body; //sequence
};

class Begin{
public:
	static string tag;
	vector <Exp> seq;
};


class Cond{  ///@@@@@@@@@@@@@@
public:
	static string tag;
};

class Application{ //@@@@ bayad env ro ham dashte bashe bara mohasebeye operand o operator
public:
	//@@@ ya mishe hesab shode bashe va maghadire nahei ke string hastan bian inja
	static string tag;
	Exp operat;// bayad hesab she
	vector <Exp> operands;
};

/*
(define (apply procedure arguments)
(cond ((primitive-procedure? procedure)
(apply-primitive-procedure procedure arguments))
((compound-procedure? procedure)
(eval-sequence
(procedure-body procedure)
(extend-environment
(procedure-parameters procedure)
arguments
(procedure-environment procedure))))
(else
(error
"Unknown procedure type: APPLY" procedure))))
*/

class Procedure{
public:
	static string tag;
	static string typePrimitive;
	static string typeCompound;
	string type; // compound procedure or primitive
	vector <string> parameters;
	vector <Exp> body;
	Environment * env;
	string namePrimitive;
};

class Comment{
public:
	static string tag;
	string text;
};


Exp eval(Exp ,Environment * );//+
void copyExp(Exp&,Exp);//+

Exp evalAssignment(Assignment,Environment*);//+
Exp evalDefinition(Definition,Environment*);//+ 
Exp evalIf(If,Environment*);//+
bool checkPredIf(If,Environment*);//+
Exp makeProcedure(Lambda,Environment*);//+
Exp evalSequence(Begin,Environment*);//+
//Exp cond2if(Cond,Environment);            //@@@@@@@@@@@@@@@@@@@@@@@@@@
Exp apply(Application,Environment*);//

Exp defineVariable(Variable , Exp ,Environment* );//+
Exp setVariableValue(Variable , Exp ,Environment *);//+
Exp lookupVariableValue(Variable,Environment*);//+
Exp addBinding(Variable,Exp,Environment*);//+
Environment * extendEnvironment(vector<string> ,vector<Exp> ,Environment *);//+


Environment initialEnv;   




Exp eval(Exp e,Environment * env){
	if(e.type==SelfEvaluating::tag)
		return e;
	if(e.type==Variable::tag)//pass by reference bar migarde dar vaghe
		return lookupVariableValue(*(Variable *)e.next,env);
	if(e.type==Quoted::tag)
		return e;//@@@@@@@@@@@@@ dorose?
	if(e.type==Assignment::tag) //set!
		return evalAssignment(*(Assignment *)e.next,env); //######### ina bayad Exp jadid dorsot konan va uno return konan
	if(e.type==Definition::tag)
		return evalDefinition(*(Definition *)e.next,env);
	if(e.type==If::tag)
		return evalIf(*(If *)e.next,env);
	if(e.type==Lambda::tag)
		return makeProcedure(*(Lambda *)e.next,env);
	if(e.type==Begin::tag)
		return evalSequence(*(Begin *)e.next,env);
	/*if(e.type==Cond::tag)//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		return eval(cond2if(*(Cond*)e.next,env),env);*/
	if(e.type==Application::tag)//else  =>pair bashe
		return apply(*(Application *)e.next,env);//@@@@@@@@@  bayad tu apply aval eval she maghadir

	cout<<"error: unknown expression type EVAL\n";
	
}

void copyExp(Exp & dest,Exp source){//***     // injuri copy sahih anjam mishe

	dest.type=source.type;

	if(source.type==SelfEvaluating::tag){
		dest.next=new SelfEvaluating;
		*((SelfEvaluating*)(dest.next))=*((SelfEvaluating * )(source.next));
	}

	if(source.type==Variable::tag){
		dest.next=new Variable;
		*(Variable*)(dest.next)=*(Variable* )(source.next);
	}

	if(source.type==Quoted::tag){
		dest.next=new Quoted;
		*(Quoted*)(dest.next)=*(Quoted* )(source.next);
	}

	if(source.type==Assignment::tag){
		dest.next=new Assignment;
		*(Assignment*)(dest.next)=*(Assignment* )(source.next);
	}

	if(source.type==Definition::tag){
		dest.next=new Definition;
		*(Definition*)(dest.next)=*(Definition* )(source.next);
	}

	if(source.type==If::tag){
		dest.next=new If;
		*(If*)(dest.next)=*(If* )(source.next);
	}

	if(source.type==Lambda::tag){
		dest.next=new Lambda;
		*(Lambda*)(dest.next)=*(Lambda* )(source.next);
	}

	if(source.type==Begin::tag){
		dest.next=new Begin;
		*(Begin*)(dest.next)=*(Begin* )(source.next);
	}

	if(source.type==Cond::tag){
		dest.next=new Cond;
		*(Cond*)(dest.next)=*(Cond* )(source.next);
	}

	if(source.type==Application::tag){
		dest.next=new Application;
		*(Application*)(dest.next)=*(Application* )(source.next);
	}

	if(source.type==Procedure::tag){
		dest.next=new Procedure;
		*(Procedure*)(dest.next)=*(Procedure* )(source.next);
	}

	if(source.type==Cond::tag){
		dest.next=new Cond;
		*(Cond*)(dest.next)=*(Cond* )(source.next);
	}
	if(source.type==Comment::tag){
		dest.next=new Comment;
		*(Comment*)(dest.next)=*(Comment* )(source.next);
	}
	


}


//!!!!!!!!!!!!!!!!!!!!!!!!!!  environment
Exp lookupVariableValue (Variable var,Environment * env){  //@@@@ pas by ref barmigarde ke mese scheme khahad bud injuri
	for(int i=0;i<env->variables.size();i++)
		if(env->variables[i]==var.name)
			return env->values[i];  
	if(env->enclosing != NULL)
		return lookupVariableValue(var,env->enclosing);
	cout<<"error: Unbound variable\n";	
	Exp tmp;
	tmp.type=Comment::tag;
	tmp.next=new Comment;
	((Comment*)tmp.next)->text="";
	return tmp;	
}

Exp defineVariable(Variable var, Exp val,Environment * env){   //@@@@ void
	Exp tmp;
	for(int i=0;i<env->variables.size();i++){
		if(env->variables[i]==var.name){
			copyExp(env->values[i],val);// val bayad ghabl azinke biad inja, eval shode bashe
			//env->values[i]=val;
			tmp.type=Comment::tag;
			tmp.next=new Comment;
			((Comment*)tmp.next)->text="'ok";
			return tmp;
		}
	}
	/*cout<<((SelfEvaluating*)val.next)->type<<endl;
	cout<<((SelfEvaluating*)val.next)->number<<endl;*/
	return addBinding(var,val,env);
}

Exp setVariableValue(Variable var, Exp val,Environment * env){  //@@@@@@@ void
	for(int i=0;i<env->variables.size();i++)
		if(env->variables[i]==var.name){
			copyExp(env->values[i],val);
			Exp tmp;
			tmp.type=Comment::tag;
			tmp.next=new Comment;
			((Comment*)tmp.next)->text="'ok";
			return tmp;
		}
	if(env->enclosing != NULL)
		return setVariableValue(var,val,env->enclosing);
	cout<<"error: Unbound variable ~set!\n";
}

Exp addBinding(Variable var,Exp val,Environment * env){
	env->variables.push_back(var.name);
	Exp tmp;
	

	/*cout<<((SelfEvaluating*)val.next)->type<<endl;
		cout<<((SelfEvaluating*)val.next)->number<<endl;*/

	copyExp(tmp,val);

	/*cout<<((SelfEvaluating*)tmp.next)->type<<endl;
	cout<<((SelfEvaluating*)tmp.next)->number<<endl;*/

	env->values.push_back(tmp);
	/*cout<<((Procedure*)tmp.next)->parameters[0]<<endl;
	cout<<((Procedure*)tmp.next)->body[0].type<<endl;
	cout<<((Variable*)(((Procedure*)tmp.next)->body[0].next))->name<<endl;*/

	//mitune 'OK bargardune

	Exp tmp2;
	tmp2.type=Comment::tag;
	tmp2.next=new Comment;
	((Comment*)tmp2.next)->text="'ok";
	return tmp2;
}


// Environment & midim chon mikhad adresesh gharar begire tu enve jadid
Environment * extendEnvironment(vector<string> vars,vector<Exp> vals,Environment * env){//ya mitune ye Exp
	Environment * res=new Environment;
	if(vars.size() < vals.size())
		cout<<"error: extend environment=> Too many arguments supplied\n" ;
	else if(vars.size() > vals.size())
		cout<<"error: extend environment=> Too few arguments supplied\n" ;
	else{
		res->enclosing= env;
		res->values=vals;
		res->variables=vars;
	}
	return  res;
}


//!!!!!!!!!!!!!!!!!!!!!!!!!  assignment & definition

//(define (eval-assignment exp env)
//(set-variable-value! (assignment-variable exp)
//(eval (assignment-value exp) env)
//env)
//’ok)


Exp evalAssignment(Assignment asn,Environment * env){//@@@@@@@@@@ 'ok
	setVariableValue(asn.var,eval(asn.value,env),env);
	Exp tmp;
	tmp.type=Comment::tag;
	tmp.next=new Comment;
	((Comment*)tmp.next)->text="'ok";
	return tmp;
}


Exp evalDefinition(Definition def,Environment * env){//@@@@@@@@ 'ok bargardune , alan alaki
	return defineVariable(def.var, eval(def.value,env) , env);
}


//!!!!!!!!!!!!!!!!!!!!!!!!!!! If

//(define (eval-if exp env)
//(if (true? (eval (if-predicate exp) env))
//(eval (if-consequent exp) env)
//(eval (if-alternative exp) env)))

Exp evalIf(If ifExpr,Environment * env){
	if(checkPredIf (ifExpr,env))
		return eval(ifExpr.consequent,env);
	else
		return eval(ifExpr.alternative,env);
}

bool checkPredIf(If ifExpr,Environment * env){
	Exp res=eval(ifExpr.predicate,env);
	if(res.type!=SelfEvaluating::tag)
		cout<<"Error: type if pred 1\n";
	else if(((SelfEvaluating *)res.next)->type != SelfEvaluating::typeNumber)
		cout<<"Error: type if pred2\n";
	else
		if( ((SelfEvaluating *)res.next)->number != 0)
			return true;
		else
			return false;
}


//!!!!!!!!!!!!!!!!!!!!!!!!!!!! procedure

//(define (make-procedure parameters body env)
//(list ’procedure parameters body env))
//(define (compound-procedure? p)
//(tagged-list? p ’procedure))
//(define (procedure-parameters p) (cadr p))
//(define (procedure-body p) (caddr p))
//(define (procedure-environment p) (cadddr p))

Exp makeProcedure(Lambda lmb,Environment * env){
	Exp res;
	res.type=Procedure::tag;
	res.next = new Procedure;
	((Procedure *)res.next)->body=lmb.body;
	((Procedure *)res.next)->env= env;
	((Procedure *)res.next)->parameters = lmb.parameters;
	((Procedure *)res.next)->type = Procedure::typeCompound;
	return res;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  sequence

//(define (eval-sequence exps env)
//(cond ((last-exp? exps)
//(eval (first-exp exps) env))
//(else
//(eval (first-exp exps) env)
//(eval-sequence (rest-exps exps) env))))

Exp evalSequence(Begin beg,Environment * env){
	for(int i=0;i<beg.seq.size()-1;i++)
		eval(beg.seq[i],env);
	return eval(beg.seq[beg.seq.size()-1],env);//@@@@ age hichi ozv nadashte bashe!
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!   apply

//(define (apply procedure arguments)
//  (cond ((primitive-procedure? procedure)
//         (apply-primitive-procedure procedure arguments))
//        ((compound-procedure? procedure)
//         (eval-sequence
//          (procedure-body procedure)
//          (extend-environment
//           (procedure-parameters procedure)
//           arguments
//           (procedure-environment procedure))))
//        (else
//         (error
//          "Unknown procedure type: APPLY" procedure))))

Exp apply(Application app,Environment * env){
	Procedure proc = *(Procedure*)(eval(app.operat,env).next);
	for(int i=0;i<app.operands.size();i++)
		app.operands[i]=eval(app.operands[i],env);
	Begin beg;
	//beg.seq=app.operands;  => ghalat bud
	beg.seq=proc.body;
	
	Exp res;
	if(proc.type==Procedure::typePrimitive){
		int val[1003];
		string valStr[1003];
		
		if( ((SelfEvaluating*)app.operands[0].next)->type == SelfEvaluating::typeNumber)
			for(int i=0;i<app.operands.size();i++)
				val[i]=((SelfEvaluating*)app.operands[i].next)->number;
		else
			for(int i=0;i<app.operands.size();i++)
				valStr[i]=((SelfEvaluating*)app.operands[i].next)->str;

		/*if(app.tag!=Variable::tag)
			cout<<"Error: apply primitive\n";*/

		res.type=SelfEvaluating::tag;
		res.next=new SelfEvaluating;
		((SelfEvaluating*)res.next)->type=SelfEvaluating::typeNumber;

		//if(((Procedure*)app.operat.next)->namePrimitive=="+"){ => in ghalat bud, chon in variable ro dare, na value
		if(proc.namePrimitive=="+"){
			((SelfEvaluating*)res.next)->number=0;
			for(int i=0;i<app.operands.size();i++)
				((SelfEvaluating*)res.next)->number+=val[i];
		}
		else if(proc.namePrimitive=="-"){
			((SelfEvaluating*)res.next)->number=val[0];
			for(int i=1;i<app.operands.size();i++)
				((SelfEvaluating*)res.next)->number-=val[i];
		}
		else if(proc.namePrimitive=="*"){
			((SelfEvaluating*)res.next)->number=1;
			for(int i=0;i<app.operands.size();i++)
				((SelfEvaluating*)res.next)->number*=val[i];
		}
		else if(proc.namePrimitive=="/"){
			((SelfEvaluating*)res.next)->number=val[0];
			for(int i=1;i<app.operands.size();i++)
				((SelfEvaluating*)res.next)->number/=val[i];
		}
		else if(proc.namePrimitive=="<"){
			if(val[0]<val[1]){
				((SelfEvaluating*)res.next)->number=1;
			}
			else
				((SelfEvaluating*)res.next)->number=0;

		}
		else if(proc.namePrimitive==">"){
			if(val[0]>val[1]){
				((SelfEvaluating*)res.next)->number=1;
			}
			else
				((SelfEvaluating*)res.next)->number=0;
		}
		else if(proc.namePrimitive=="="){
			if(app.operands[0].type==SelfEvaluating::tag){
				if( ((SelfEvaluating*)app.operands[0].next)->type == SelfEvaluating::typeNumber){
					if(val[0]==val[1]){
						((SelfEvaluating*)res.next)->number=1;
					}
					else
						((SelfEvaluating*)res.next)->number=0;
				}
				else{
					if(valStr[0]==valStr[1]){
						((SelfEvaluating*)res.next)->number=1;
					}
					else
						((SelfEvaluating*)res.next)->number=0;
				}
			}
						
			else if(app.operands[0].type==Quoted::tag){ 
				cout<<((Quoted*)app.operands[0].next)->name<<' '<<((Quoted*)app.operands[1].next)->name<<endl;
				if( ((Quoted*)app.operands[0].next)->name == ((Quoted*)app.operands[1].next)->name )
					((SelfEvaluating*)res.next)->number=1;
				else
					((SelfEvaluating*)res.next)->number=0;
			}
		}
		

	}
	else if(proc.type==Procedure::typeCompound){
		return evalSequence(beg,extendEnvironment(proc.parameters,app.operands,env));
	}
	return res;

}





//meghdar dehie avalie be tag ha:
string SelfEvaluating::tag("self evaluating");
string SelfEvaluating::typeNumber("selfNumber");
string SelfEvaluating::typeStr("selfString");
string Variable::tag("variable");
string Quoted::tag("qouted");
string Assignment::tag("assignment");
string Definition::tag("definition");
string If::tag("if");
string Lambda::tag("lambda");
string Begin::tag("begin");
string Cond::tag("cond");
string Application::tag("application");
string Environment::tag("environment");//@@@@@@@@@@ lazeme?
string Procedure::tag("procedure");
string Procedure::typePrimitive("primitive proc");
string Procedure::typeCompound("compound proc");
string Comment::tag("comment");

char command[10004];
vector <string> tokenCom;
int numOpenParantes;  //tedade parantezhai ke baz shodan va hanuz base nashodano neshun mide


//!!!!!!!!!!!!!!!!!!!!!!!!!!!    syntax avalie


int numberToken(string tok){
	int res=0;
	for(int i=0;i<tok.size();i++){
		res*=10;
		res+=tok[i]-'0';
	}
	return res;
}

bool isNumberToken(string tok){
	for(int i=0;i<tok.size();i++){
		if(tok[i]<'0' || tok[i]>'9')
			return false;
	}
	return true;
}

bool isStringToken(string tok){
	if((tok[0]=='"' && tok[tok.size()-1]=='"') || tok[0]=='\'')
		return true;
	return false;
}

bool isVariable(string tok){
	if(tok!="(") //@@@@@@@@@@@@@@@@@@@@@ dorose?
		return true;
	else
		return false;
}

Exp analyseToken(int & index){
	Exp res;
	if(isNumberToken(tokenCom[index])){
		res.type=SelfEvaluating::tag;
		res.next=new SelfEvaluating;
		((SelfEvaluating*)(res.next))->type=SelfEvaluating::typeNumber;
		((SelfEvaluating*)(res.next))->number=numberToken(tokenCom[index]);
		index++;
		return res;
	}
	else if(isStringToken(tokenCom[index])){
		res.type=SelfEvaluating::tag;
		res.next=new SelfEvaluating;
		((SelfEvaluating *)(res.next))->type=SelfEvaluating::typeStr;
		((SelfEvaluating *)(res.next))->str=tokenCom[index];
		index++;
		return res;
	}

	else if(isVariable(tokenCom[index])){ // (  nist
		res.type= Variable::tag;
		res.next=new Variable;
		((Variable*)res.next)->name=tokenCom[index];
		index++;
		return res;
	}

	else if(tokenCom[index]=="("){
		//res=analyseToken(index+1);//@@@@@@@@@@@@@@@@
		if(tokenCom[index+1]=="define"){
			res.type= Definition::tag;
			res.next=new Definition;
			index+=2;

			if(tokenCom[index]!="("){ // variable
				((Definition *)res.next)->var = *((Variable*)analyseToken(index).next);
				((Definition *)res.next)->value = analyseToken(index);
				if(tokenCom[index]==")"){
					index++;
					return res;
				}
				else
					cout<<"Error: analyseToken difinition\n";
			}
			else{ // syntax suger baraye tabe
				index++; // "("
				((Definition *)res.next)->var = *((Variable*)analyseToken(index).next);
				//((Lambda*)(((Definition *)res.next)->value))->
				Exp & valueExp=(((Definition*)res.next)->value);
				valueExp.type=Lambda::tag;
				valueExp.next=new Lambda;

				while(tokenCom[index]!=")"){
					((Lambda*)valueExp.next)->parameters.push_back(tokenCom[index++]);//++ ham shode
				}
				index++;// bara ")"
				while(tokenCom[index]!=")"){//akhare define e avlie
					((Lambda*)valueExp.next)->body.push_back(analyseToken(index));
				}
				index++;
				return res;
			}
		}
		else if(tokenCom[index+1]=="set!"){
			res.type=Assignment::tag;
			res.next=new Assignment;
			index+=2;
			((Assignment*)res.next)->var=  *((Variable*)analyseToken(index).next);
			((Assignment*)res.next)->value=analyseToken(index);
			if(tokenCom[index]==")"){
				index++;
				return res;
			}
			else
				cout<<"Error: analyseToken assignment\n";
		}
		else if(tokenCom[index+1]=="quote"){
			res.type=Quoted::tag;
			res.next=new Quoted;
			index+=2;
			((Quoted*)res.next)->name="'";
			((Quoted*)res.next)->name.append( tokenCom[index]);
			index++;//marbut be ((Quoted*)res.next)->name ke khundim
			if(tokenCom[index]==")"){
				index++;
				return res;
			}
			else
				cout<<"Error: analyseToken qoute\n";
		}
		else if(tokenCom[index+1]=="if"){
			res.type=If::tag;
			res.next=new If;
			index+=2;
			((If*)res.next)->predicate = analyseToken(index);
			((If*)res.next)->consequent= analyseToken(index);
			((If*)res.next)->hasAlternative=false;

			if(tokenCom[index]!=")"){ //alternative dare
				((If*)res.next)->alternative= analyseToken(index);
				((If*)res.next)->hasAlternative=true;
			}
			if(tokenCom[index]==")"){
				index++;
				return res;
			}
			else
				cout<<"Error: analyseToken if\n";
		}
		else if(tokenCom[index+1]=="cond"){
			res.type=If::tag;
			res.next=new If;
			index+=2;
			If * ifTmp=(If *) res.next;
			while(tokenCom[index]!=")"){
				if(tokenCom[index]=="(")
					index++;
				else
					cout<<"Error: analyseToken cond 1\n";
				if(tokenCom[index]=="else"){
					ifTmp->predicate.type=SelfEvaluating::tag;
					ifTmp->predicate.next=new SelfEvaluating;
					((SelfEvaluating*)(ifTmp->predicate.next))->type = SelfEvaluating::typeNumber;
					((SelfEvaluating*)(ifTmp->predicate.next))->number = 1;
					index++;
				}
				else
					ifTmp->predicate = analyseToken(index);
				ifTmp->consequent = analyseToken(index);
				if(tokenCom[index]==")")
					index++;
				else
					cout<<"Error: analyseToken cond 2\n";
				ifTmp->alternative.type=If::tag;
				(ifTmp->alternative).next=new If;
				ifTmp=(If *)(ifTmp->alternative).next;
			}
			index++;// ")" e akhare cond
			return res;
		}

		else if(tokenCom[index+1]=="begin"){
			res.type=Begin::tag;
			res.next=new Begin;
			index+=2;
			while(tokenCom[index]!=")"){
				((Begin*)res.next)->seq.push_back(analyseToken(index));
			}
			index++;
			return res;  //else => dar nazar nagereftam dige, nemikhad aslan
		}
		else if(tokenCom[index+1]=="lambda"){
			res.type=Lambda::tag;
			res.next=new Lambda;
			index+=2;
			if(tokenCom[index]=="("){
				index++;//bara '('
				while(tokenCom[index]!=")"){
					((Lambda*)res.next)->parameters.push_back(tokenCom[index++]);
				}
				index++; //bara ")"
				while(tokenCom[index]!=")"){
					((Lambda*)res.next)->body.push_back(analyseToken(index));
				}
				index++;
				return res;
			}
			else
				cout<<"Error analyseToken Lambda 1\n";
			
		}
		else{// seda kardane tabe
			index++;
			res.type=Application::tag;
			res.next=new Application;

			((Application*)res.next)->operat=analyseToken(index);
			while(tokenCom[index]!=")"){
				(((Application*)res.next)->operands).push_back(analyseToken(index));
			}
			index++;// ")"
			return res;
			
			
		}

	}

}

void print(Exp ex){
	if(ex.type==SelfEvaluating::tag){
		if(((SelfEvaluating*)ex.next)->type == SelfEvaluating::typeNumber)
			cout<<((SelfEvaluating*)ex.next)->number;
		else if(((SelfEvaluating*)ex.next)->type == SelfEvaluating::typeStr)
			cout<<((SelfEvaluating*)ex.next)->str;
		cout<<endl;
	}

	if(ex.type==Variable::tag){
		cout<<(*(Variable*)ex.next).name<<endl;
	}

	if(ex.type==Quoted::tag){
		cout<< (*(Quoted*)ex.next).name<<endl;
	}

	if(ex.type==Comment::tag)
		cout<<((Comment*)ex.next)->text<<endl;

	/*if(ex.type==Assignment::tag){
		Assignment a;
		a.
		*(Assignment*)(dest.next)
	}

	if(ex.type==Definition::tag){
		dest.next=new Definition;
		*(Definition*)(dest.next)=*(Definition* )(ex.next);
	}

	if(ex.type==If::tag){
		dest.next=new If;
		*(If*)(dest.next)=*(If* )(ex.next);
	}

	if(ex.type==Lambda::tag){
		dest.next=new Lambda;
		*(Lambda*)(dest.next)=*(Lambda* )(ex.next);
	}

	if(ex.type==Begin::tag){
		dest.next=new Begin;
		*(Begin*)(dest.next)=*(Begin* )(ex.next);
	}

	if(ex.type==Cond::tag){
		dest.next=new Cond;
		*(Cond*)(dest.next)=*(Cond* )(ex.next);
	}

	if(ex.type==Application::tag){
		dest.next=new Application;
		*(Application*)(dest.next)=*(Application* )(ex.next);
	}*/
	
	
}

void tokenize(){
	/*if(tokenComAnalyse==true)
		tokenCom.clear();*/
	int commandSize=strlen(command);
	command[commandSize++]=' ';
	command[commandSize]='\0';
	string last;

	

	for(int i=0;i<commandSize;i++){
		if(command[i]==';')
			return;
		int tokenIndex=0;
		if(command[i]==' ' || command[i]=='\n' || command[i]=='\t'){
			if(last!=""){
				tokenCom.push_back(last);
				last="";
			}
		}
		else if(command[i]=='('){
			if(last!="")
				tokenCom.push_back(last);
			tokenCom.push_back("(");
			last="";
			numOpenParantes++;
			
		}
		else if(command[i]==')'){
			if(last!="")
				tokenCom.push_back(last);
			tokenCom.push_back(")");
			last="";
			numOpenParantes--;
		}
		else{
			last.push_back(command[i]);
		}
		if(numOpenParantes==0 && (command[i]==' ' || command[i]=='\t' || command[i]=='\n' || command[i]==')' || command[i]=='(') && tokenCom.size()>0){
			print(eval(analyseToken(tokenIndex),&initialEnv));
			tokenCom.clear();
		}


	}
 }


void addPrimitives(Environment * env){ 
	Variable var;
	Exp val;
	val.type=Procedure::tag;
	val.next=new Procedure;
	

	var.name="+";	
	((Procedure*)val.next)->type=Procedure::typePrimitive;
	((Procedure * )val.next)->namePrimitive="+";
	addBinding(var,val,env);

	var.name="-";	
	((Procedure * )val.next)->namePrimitive="-";
	addBinding(var,val,env);

	var.name="*";	
	((Procedure * )val.next)->namePrimitive="*";
	addBinding(var,val,env);

	var.name="/";	
	((Procedure * )val.next)->namePrimitive="/";
	addBinding(var,val,env);

	var.name=">";	
	((Procedure * )val.next)->namePrimitive=">";
	addBinding(var,val,env);

	var.name="<";	
	((Procedure * )val.next)->namePrimitive="<";
	addBinding(var,val,env);

	var.name="=";	
	((Procedure * )val.next)->namePrimitive="=";
	addBinding(var,val,env);
}


int main(){
	

	freopen("input.txt","r",stdin);

	initialEnv.enclosing=NULL;
	addPrimitives(&initialEnv);
	//@@@@@@@@   primitive proc ha bayad ezafe beshan
	

	while(gets(command))
		tokenize();

	return 0;
}