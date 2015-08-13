#include "simpleList.h"
  
void simpleList::setParent(simpleList& list){
  list.tail = this;
  this->front = &list;
}
void simpleList::setChild(simpleList& list){
  list.setParent(*this);
}

bool simpleList::isLast(){
  return tail==0;
}
bool simpleList::isFirst(){
  return front==0;
}
simpleList * simpleList::next(){
  return tail;
}
simpleList * simpleList::prev(){
  return front;
}

simpleList::simpleList() : tail(0),front(0){
  
}