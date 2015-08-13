
class simpleList {
  simpleList * tail;
  simpleList * front;
public:
  void setParent(simpleList& list);
  void setChild(simpleList& list);
  
  bool isLast();
  bool isFirst();
  simpleList * next();
  simpleList * prev();
  simpleList();
};