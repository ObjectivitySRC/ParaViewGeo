#include <string>
#include <iostream>
#include <vector>

namespace CLIPS
{
#define CLIPS_COMPILED_AS_C_PLUS_PLUS 0

class CLIPSCPPRouter;

class DataObject;

class CLIPSCPPEnv
  {
   private:
      void *theEnv;

   public:
      CLIPSCPPEnv();
      ~CLIPSCPPEnv();
      void Clear();
      int Load(char *);
      void Reset();
      long long Run(long long runLimit = -1);
      int Watch(char *);
      int Unwatch(char *);
      DataObject Eval(char *);
      bool Build(char *);
      int AddRouter(char *,int,CLIPSCPPRouter *);
  };

class CLIPSCPPRouter
  {
   public:
      virtual int Query(CLIPSCPPEnv *,char *);
      virtual int Print(CLIPSCPPEnv *,char *,char *);
      virtual int Getc(CLIPSCPPEnv *,char *);
      virtual int Ungetc(CLIPSCPPEnv *,int,char *);
      virtual int Exit(CLIPSCPPEnv *,int);
  };
  
class Value
  {
   public:
     Value();
     Value(const Value& v);
     virtual ~Value();
     /* virtual Value& operator= (const Value& v); */
     friend std::ostream& operator<< (std::ostream& o, const Value& s);
     friend std::ostream& operator<< (std::ostream& o, const Value* s);
     virtual std::ostream& print(std::ostream& o) const = 0;
     virtual Value *clone() const = 0; 
     virtual long long GetFactIndex() const;
     virtual DataObject GetFactSlot(char *) const;

   private:
     virtual char *GetRawInstanceName() const; 
  };

class VoidValue : public Value
  {  
   public:
     VoidValue();
     VoidValue(const VoidValue& v);
     virtual ~VoidValue();
     /* virtual VoidValue& operator= (const VoidValue& s); */
     virtual std::ostream& print(std::ostream& o) const;
     virtual VoidValue *clone() const; 
  };

class StringValue : public Value
  {  
   public:
     StringValue();
     StringValue(char *);
     StringValue(const StringValue& v);
     virtual ~StringValue();
     /* virtual StringValue& operator= (const StringValue& v); */
     virtual std::ostream& print(std::ostream& o) const;
     virtual StringValue *clone() const; 

   private:
     std::string theString;
  };

class SymbolValue : public Value
  { 
   public:
     SymbolValue();
     SymbolValue(char *);
     SymbolValue(const SymbolValue& v);
     virtual ~SymbolValue();
     /* virtual SymbolValue& operator= (const SymbolValue& v); */
     virtual std::ostream& print(std::ostream& o) const;
     virtual SymbolValue *clone() const; 
  
   private:
     std::string theString;
  };

class InstanceNameValue : public Value
  { 
   public:
     InstanceNameValue();
     InstanceNameValue(char *);
     InstanceNameValue(const InstanceNameValue& v);
     virtual ~InstanceNameValue();
     /* virtual InstanceNameValue& operator= (const InstanceNameValue& v); */
     virtual std::ostream& print(std::ostream& o) const;
     virtual InstanceNameValue *clone() const; 
  
   private:
     std::string theString;
  };

class IntegerValue : public Value
  { 
   public:
     IntegerValue();
     IntegerValue(long long);
     IntegerValue(const IntegerValue& v);
     virtual ~IntegerValue();
     /* virtual IntegerValue& operator= (const IntegerValue& v); */
     virtual std::ostream& print(std::ostream& o) const;
     virtual IntegerValue *clone() const; 
  
   private:
     long long theInteger;
  };

class FloatValue : public Value
  { 
   public:
     FloatValue();
     FloatValue(double);
     FloatValue(const FloatValue& v);
     virtual ~FloatValue();
     /* virtual FloatValue& operator= (const FloatValue& v); */
     virtual std::ostream& print(std::ostream& o) const;
     virtual FloatValue *clone() const; 
  
   private:
     double theFloat;
  };

class FactAddressValue : public Value
  { 
   public:
     FactAddressValue(void *,void *);
     FactAddressValue(const FactAddressValue& v);
     virtual ~FactAddressValue();
     virtual FactAddressValue& operator= (const FactAddressValue& v);
     virtual std::ostream& print(std::ostream& o) const;
     virtual FactAddressValue *clone() const; 
     virtual DataObject GetFactSlot(char *) const;
  
   private:
     virtual long long GetFactIndex() const;
     void *theEnvironment;
     void *theFactAddress;
  };
 
class InstanceAddressValue : public Value
  { 
   public:
     InstanceAddressValue(void *,void *);
     InstanceAddressValue(const InstanceAddressValue& v);
     virtual ~InstanceAddressValue();
     virtual InstanceAddressValue& operator= (const InstanceAddressValue& v);
     virtual std::ostream& print(std::ostream& o) const;
     virtual InstanceAddressValue *clone() const; 
  
   private:
     virtual char *GetRawInstanceName() const;
     void *theEnvironment;
     void *theInstanceAddress;
  };
  
class MultifieldValue : public Value
  {  
   public:
     MultifieldValue();
     MultifieldValue(size_t);
     MultifieldValue(const MultifieldValue& v);
     virtual ~MultifieldValue();
     virtual MultifieldValue& operator= (const MultifieldValue& v);
     virtual std::ostream& print(std::ostream& o) const;
     virtual MultifieldValue *clone() const; 
     void add(Value *);

   private:
     std::vector<Value *> theMultifield;
  };

class DataObject
  {
   public:
     DataObject();
     DataObject(Value *v);
     DataObject(const DataObject& v);
     virtual ~DataObject();
     virtual DataObject& operator= (const DataObject& s);
     friend std::ostream& operator<< (std::ostream& o, const DataObject& s);
     friend std::ostream& operator<< (std::ostream& o, const DataObject* s);
     virtual std::ostream& print(std::ostream& o) const;
     virtual DataObject GetFactSlot(char *) const;
     static DataObject Void();
     static DataObject String();
     static DataObject String(char *);
     static DataObject Symbol();
     static DataObject Symbol(char *);
     static DataObject InstanceName();
     static DataObject InstanceName(char *);
     static DataObject Multifield();
     static DataObject Multifield(size_t);
     static DataObject Integer();
     static DataObject Integer(long long);
     static DataObject Float();
     static DataObject Float(double);
     static DataObject FactAddress(void *,void *);
     static DataObject InstanceAddress(void *,void *);

   private:
     Value *theValue;
  };

inline DataObject DataObject::Void()
  { return DataObject(); }

inline DataObject DataObject::String()
  { return DataObject(new StringValue()); }

inline DataObject DataObject::String(
  char *initialString)
  { return DataObject(new StringValue(initialString)); }

inline DataObject DataObject::Symbol()
  { return DataObject(new SymbolValue()); }

inline DataObject DataObject::Symbol(
  char *initialString)
  { return DataObject(new SymbolValue(initialString)); }

inline DataObject DataObject::InstanceName()
  { return DataObject(new InstanceNameValue()); }

inline DataObject DataObject::InstanceName(
  char *initialString)
  { return DataObject(new InstanceNameValue(initialString)); }

inline DataObject DataObject::Multifield()
  { return DataObject(new MultifieldValue()); }

inline DataObject DataObject::Multifield(size_t size)
  { return DataObject(new MultifieldValue(size)); }

inline DataObject DataObject::Integer()
  { return DataObject(new IntegerValue(0)); }

inline DataObject DataObject::Integer(long long theInteger)
  { return DataObject(new IntegerValue(theInteger)); }

inline DataObject DataObject::Float()
  { return DataObject(new FloatValue(0.0)); }

inline DataObject DataObject::Float(double theFloat)
  { return DataObject(new FloatValue(theFloat)); }

inline DataObject DataObject::FactAddress(void *theEnv,void *theFact)
  { return DataObject(new FactAddressValue(theEnv,theFact)); }

inline DataObject DataObject::InstanceAddress(void *theEnv,void *theInstance)
  { return DataObject(new InstanceAddressValue(theEnv,theInstance)); }

}
