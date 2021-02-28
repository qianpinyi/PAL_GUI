#ifndef PAL_SPLAYTREE_CPP
#define PAL_SPLAYTREE_CPP 1 

#include "../PAL_BasicFunctions/PAL_BasicFunctions_0.cpp"
#include "../PAL_BasicFunctions/PAL_BasicFunctions_Debug.cpp"

template <class T> class SplayTree;

template <class T> class SplayTreeNode
{
	friend class SplayTree <T>;
	protected:
		static SplayTreeNode<T> * const nullNode;

		int size=0;
		SplayTreeNode *l=NULL,*r=NULL,*fa=NULL;
					  
		void Zig()//Left rotate
		{
			if (r==nullNode) return;
			SplayTreeNode *te=r;
			te->size=size;
			size-=te->r->size+1;
			if (this==fa->l) fa->l=te;
			else fa->r=te;
			te->fa=fa;
			fa=te;
			te->l->fa=this;
			r=te->l;
			te->l=this;
		}
		
		void Zag()
		{
			if (l==nullNode) return;
			SplayTreeNode *te=l;
			te->size=size;
			size-=te->l->size+1;
			if (this==fa->l) fa->l=te;
			else fa->r=te;
			te->fa=fa;
			fa=te;
			te->r->fa=this;
			l=te->r;
			te->r=this;
		}
	
		SplayTreeNode<T>* Splay(SplayTreeNode *s)
		{
			if (this==nullNode) return nullNode;
			s=s->fa;
			while (fa!=s)
			{
				SplayTreeNode *ff=fa->fa;
				if (ff==s)
					if (this==fa->l) fa->Zag();
					else fa->Zig();
				else if (this==fa->l&&fa==ff->l)
					ff->Zag(),fa->Zag();
				else if (this==fa->r&&fa==ff->r)
					ff->Zig(),fa->Zig();
				else if (this==fa->l&&fa==ff->r)
					fa->Zag(),ff->Zig();
				else if (this==fa->r&&fa==ff->l)
					fa->Zig(),ff->Zag();
			}
			return this;
		}
		
		SplayTreeNode<T>* GetKth(int k)
		{
			if (this==nullNode) return this;
			if (l->size>=k) return l->GetKth(k);
			else if (l->size==k-1) return this;
			else return r->GetKth(k-l->size-1);
		}
		
		SplayTreeNode() {}
		
		SplayTreeNode(int _size,SplayTreeNode *_l,SplayTreeNode *_r,SplayTreeNode *_fa)
		:size(_size),l(_l),r(_r),fa(_fa) {}
	
		~SplayTreeNode()//It will delete the subTree
		{
			if (l!=nullNode)
				delete l;
			if (r!=nullNode)
				delete r;
		}
		
		SplayTreeNode(int L,int R,const T src[])
		{
			data=src[L+R>>1];
			size=1;
			fa=nullNode;
			if (L<=(L+R>>1)-1)
			{
				l=new SplayTreeNode<T>(L,(L+R>>1)-1,src);
				l->fa=this;
				size+=l->size;
			}
			else l=nullNode;
			if (R>=(L+R>>1)+1)
			{
				r=new SplayTreeNode<T>((L+R>>1)+1,R,src);
				r->fa=this;
				size+=r->size;
			}
			else r=nullNode;
		}
		
	public:
		T data;
		
		T& operator () ()
		{return data;}
		
		SplayTreeNode(const T &_data):data(_data)
		{
			size=1;
			l=r=fa=nullNode;
		}
};
template <class T> SplayTreeNode <T> * const SplayTreeNode<T>::nullNode=new SplayTreeNode<T>();

template <class T> class SplayTree
{
	protected:
		SplayTreeNode <T> *root;

		SplayTreeNode <T>* GetPre(SplayTreeNode <T> *tar)
		{
			tar->Splay(root->r);
			return tar->l;
		}
		
		SplayTreeNode <T>* GetNxt(SplayTreeNode <T> *tar)
		{
			tar->Splay(root->r);
			return tar->r;
		}
		
	public:
		void DebugChecknullNode()
		{
			DD<<"[SplayTreeDebug]: nullNode:"<<endl
			  <<"size:"<<SplayTreeNode<T>::nullNode->size<<endl
			  <<"fa is NULL:"<<(SplayTreeNode<T>::nullNode->fa==0)<<endl
			  <<"l is NULL:"<<(SplayTreeNode<T>::nullNode->l==0)<<endl
			  <<"r is NULL:"<<(SplayTreeNode<T>::nullNode->r==0)<<endl;
		}
		
		inline int size()
		{return root->r->size;}
		
		SplayTreeNode<T>* const begin()
		{return root->r->GetKth(1);}
		
		SplayTreeNode<T>* const end()
		{return SplayTreeNode<T>::nullNode;}
		
		SplayTreeNode<T>* const rbegin()
		{return root->r->GetKth(root->r->size);}
		
		void insert(int p,const T &src)
		{
			if (root->r->size==0)
			{
				root->r=new SplayTreeNode<T>(src);
				root->r->fa=root;
			}
			else if (p<=0)
			{
				root->r->GetKth(1)->Splay(root->r);
				root->r->l=new SplayTreeNode<T>(src);
				root->r->l->fa=root->r;
				root->r->size++;
			}
			else if (p>=root->r->size)
			{
				root->r->GetKth(root->r->size)->Splay(root->r);
				root->r->r=new SplayTreeNode<T>(src);
				root->r->r->fa=root->r;
				root->r->size++;
			}
			else
			{
				SplayTreeNode<T> *stn=new SplayTreeNode<T>(src);
				root->r->GetKth(p)->Splay(root->r);
				root->r->GetKth(p+1)->Splay(root->r->r);
				stn->fa=root->r->r;
				root->r->r->l=stn;
				root->r->r->size++;
				root->r->size++;
			}
		}
		
		void insert(int p,int L,int R,const T src[])
		{
			if (L>R) return;
			if (root->r->size==0)
			{
				root->r=new SplayTreeNode<T>(L,R,src);
				root->r->fa=root;
			}
			else if (p<=0)
			{
				root->r->GetKth(1)->Splay(root->r);
				root->r->l=new SplayTreeNode<T>(L,R,src);
				root->r->l->fa=root->r;
				root->r->size+=root->r->l->size;
			}
			else if (p>=root->r->size)
			{
				root->r->GetKth(root->r->size)->Splay(root->r);
				root->r->r=new SplayTreeNode<T>(L,R,src);
				root->r->r->fa=root->r;
				root->r->size+=root->r->r->size;
			}
			else
			{
				SplayTreeNode<T> *stn=new SplayTreeNode<T>(L,R,src);
				root->r->GetKth(p)->Splay(root->r);
				root->r->GetKth(p+1)->Splay(root->r->r);
				stn->fa=root->r->r;
				root->r->r->l=stn;
				root->r->r->size+=stn->size;
				root->r->size+=stn->size;
			}
		}
		
		void insert(int p,int len,const T src[])
		{insert(p,0,len-1,src);}
		
		void insert(int p,SplayTree <T> *src)
		{
			
		}
				
		void erase(int p,int len)
		{
			if (root->r->size==0) return;
			p=EnsureInRange(p,0,root->r->size-1);
			len=EnsureInRange(len,1,root->r->size-p);
			if (p==0&&len==root->r->size)
			{
				delete root->r;
				root->r=SplayTreeNode<T>::nullNode;
			}
			else if (p==0)
			{
				root->r->GetKth(p+len+1)->Splay(root->r);
				delete root->r->l;
				root->r->size-=len;
				root->r->l=SplayTreeNode<T>::nullNode;
			}
			else if (p+len==root->r->size)
			{
				root->r->GetKth(p)->Splay(root->r);
				delete root->r->r;
				root->r->size-=len;
				root->r->r=SplayTreeNode<T>::nullNode;
			}
			else
			{
				root->r->GetKth(p)->Splay(root->r);
				root->r->GetKth(p+len+1)->Splay(root->r->r);
				delete root->r->r->l;
				root->r->r->size-=len;
				root->r->size-=len;
				root->r->r->l=SplayTreeNode<T>::nullNode;
			}
		}
		
		void erase(int p)
		{erase(p,1);}
		
		void clear()
		{erase(0,root->r->size);}
		
		T& get(int p)
		{return root->r->GetKth(p+1)->Splay(root->r)->data;}
		
		T& operator [] (int p)//count from 0,if x is not in range,it will be fixed autoly
		{return root->r->GetKth(p+1)->Splay(root->r)->data;}
		
		~SplayTree()
		{
			delete root;
		}
		
		SplayTree(int len,const T src[])
		{
			root=new SplayTreeNode <T> (0,SplayTreeNode<T>::nullNode,SplayTreeNode<T>::nullNode,SplayTreeNode<T>::nullNode);
			insert(0,len,src);
		}
		
//		SplayTree(const SplayTree &tar)
//		{
//			DD<<"[SplayTreeDebug] Cannot use yet"<<endl;
//		}
		
		SplayTree()
		{
			root=new SplayTreeNode <T> (0,SplayTreeNode<T>::nullNode,SplayTreeNode<T>::nullNode,SplayTreeNode<T>::nullNode);
			
		}
};
#endif
