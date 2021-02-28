#ifndef PAL_TRIETREE_CPP
#define PAL_TRIETREE_CPP 1 

#include <string>

template <class T> class TrieTree
{
	public:
		struct TrieTreeNode
		{
			friend class TrieTree;
			protected:
				char ch=0;
				int count=0;
				TrieTreeNode *fa=NULL,
							 *child=NULL,
							 *nxt=NULL;
				T *data=NULL;
		
				TrieTreeNode(char _ch)
				:ch(_ch) {}
				
				TrieTreeNode() {}
				
			public:
//				TrieTreeNode* GetNextDataNode()
//				{
//					
//				}
				
				T& operator () ()
				{return *data;}
				
				T& GetData()
				{return *data;}
		};
		
	protected:
		TrieTreeNode *rootNode=NULL;
		
		TrieTreeNode* _Find(TrieTreeNode *p,const std::string &key,int q)
		{
			if (q==key.length())
				if (p->data==NULL) return NULL;
				else return p;
			for (TrieTreeNode *i=p->child;i;i=i->nxt)
				if (i->ch==key[q])
					return _Find(i,key,q+1);
			return NULL;
		}
		
		TrieTreeNode* _Insert(TrieTreeNode *p,const std::string &key,int q,const T &value)//0:fail 1:success
		{
			if (q==key.length())
			{
				if (p->data!=NULL)
					return NULL;
				p->data=new T(value);
				p->count=1;
				return p;
			}
			for (TrieTreeNode *i=p->child;i;i=i->nxt)
				if (i->ch==key[q])
				{
					TrieTreeNode *re=_Insert(i,key,q+1,value);
					if (re!=NULL)
						++p->count;
					return re;
				}
			TrieTreeNode *node=new TrieTreeNode(key[q]);
			node->fa=p;
			if (p->child==NULL||node->ch<p->child->ch)
				node->nxt=p->child,
				p->child=node;
			else
			{
				TrieTreeNode *pp=p->child;
				while (pp->nxt)
					if (pp->nxt->ch>node->ch)
					{
						node->nxt=pp->nxt;
						pp->nxt=node;
						break;
					}
					else pp=pp->nxt;
				if (pp->nxt==NULL)
					pp->nxt=node;
			}
			++p->count;
			return _Insert(node,key,q+1,value);
		}
		
		void Deconstruct(TrieTreeNode *p)
		{
			if (p->nxt!=NULL)
				Deconstruct(p->nxt);
			if (p->child!=NULL)
				Deconstruct(p->child);
			if (p->data!=NULL)
				delete p->data;
			delete p;
		}
		
		TrieTreeNode* _FindHead(TrieTreeNode *p)
		{
			if (p==NULL)
				return NULL;
			if (p->data!=NULL)
				return p;
			return _FindHead(p->child);
		}
		
		void _GetAllNode(TrieTreeNode *p,void(*func)(void*,const std::string&,T&),void *funcdata,std::string &str)
		{
			if (p->data!=NULL)
				func(funcdata,str,*(p->data));
			for (TrieTreeNode *i=p->child;i;i=i->nxt)
			{
				str+=i->ch;
				_GetAllNode(i,func,funcdata,str);
				str.erase(str.length()-1,1);
			}
		}
		
	public:
		int GetAllNode(void(*func)(void*,const std::string&,T&),void *funcdata=NULL)
		{
			std::string str;
			_GetAllNode(rootNode,func,funcdata,str);
			return rootNode->count;
		}
		
		TrieTreeNode *Insert(const std::string &key,const T &value)
		{return _Insert(rootNode,key,0,value);}
		
		TrieTreeNode* Find(const std::string &key)
		{return _Find(rootNode,key,0);}
		
		void Erase(const std::string &key)
		{
			TrieTreeNode *p=Find(key),*q;
			if (p==NULL)
				return;
			delete p->data;
			p->data=NULL;
			bool flag=1;
			while (p!=rootNode)
			{
				--p->count;
				if (p->count>0)
					flag=0;
				q=p;
				p=p->fa;
				if (flag)
				{
					if (p->child==q)
						p->child=q->nxt;
					else 
						for (TrieTreeNode *i=p->child;;i=i->nxt)
							if (i->nxt==q)
							{
								i->nxt=q->nxt;
								break;
							}
					delete q;
				}
			}
			--rootNode->count;
		}
		
//		TrieTreeNode* FindHead()
//		{return _FindHead(rootNode);}
		
		inline int GetCount()
		{return rootNode->count;}
		
		void Clear()
		{
			Deconstruct(rootNode);
			rootNode=new TrieTreeNode;
		}

		~TrieTree()
		{Deconstruct(rootNode);}
		
		TrieTree()
		{rootNode=new TrieTreeNode;}
};

#endif
