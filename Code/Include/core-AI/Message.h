#pragma once


///////////////////////////////////////////////////////////////////////////////

class Messagable;

///////////////////////////////////////////////////////////////////////////////

class Message
{
public:
   virtual ~Message() {}

   virtual Message* clone() = 0;
};

///////////////////////////////////////////////////////////////////////////////

template<typename T>
class TMessage
{
public:
   virtual ~TMessage() {}

   virtual void perform(T& receiver, Messagable& sender) = 0;
};

///////////////////////////////////////////////////////////////////////////////