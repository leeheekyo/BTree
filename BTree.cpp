/**
 * B-Tree 예제 파일
 * Author : 이희교
 * create date : 2018.12.30
 * description 
 *   : 인터넷에 나와있는 소스코드를 참고하여 만들었으며 부족한 부분은 직접 수정하여 작성하였습니다. 
 *     개인적으로 공부하는 목적 이외에 다른 사이트 기재하는 것을 원할 경우 개인적으로 연락주시가 바랍니다.
 * contact : glhk5895@naver.com 
 * reference : https://www.geeksforgeeks.org/b-tree-set-3delete/ 
 */
#include<iostream> 

#define DEGREE 5 
#define MIN_NODE (DEGREE - 1 ) / 2

using namespace std;

// BTree의 각각의 Node 
class BTreeNode {
    int *keys;     // 저장하는 값, 배열로 보관
    BTreeNode **C; // 자식의 노드를 저장하는 이차원 배열 
    int n;         // 현재 노드의 키 값의 개수
    bool leaf;     // 자식노드가 없을 경우 true, 아닐 경우 false 
public: 
    // 생성자 
    BTreeNode(bool _leaf);

    // 현재 아래의 모든 노드를 출력 
    void traverse(); 

    // 키 값이 있는지 찾는 함수, 못 찾을 경우 NULL 리턴 
    BTreeNode *search(int k);

    // 현재 노드에 키를 넣을 공간이 있을 경우 호출 
    void insert(int k); 
  
    // 
    void splitChild(int i);
    
    //
    void splitRoot(); 
  
    // 키 값에 해당하는 값을 삭제 
    void remove(int k); 
  
    // idx 번째 자식 노드 중 가장 큰 값을 가져온다. 
    int getPrev(int idx); 
  
    // idx 번째 자식 노드 중 가장 작은 값을 가져온다. 
    int getNext(int idx); 
  
    // idx 번째 값을 채움( idx와 idx+1에서 찾아서 사용, 없을 시 merge함수를 사용 ) 
    void fill(int idx); 
  
    // idx 번째 자식 노드가 부족할 때 호출, idx-1 번째 자식의 제일 큰 키 값을 idx-1 번째 키로 idx 번째 키를 idx 번째 자식의 0번째 key로 입력 
    void borrowFromPrev(int idx); 
  
    // idx 번째 자식 노드가 부족할 때 호출, idx+1 번째 자식의 제일 작은 키 값을 idx 번째 키로 idx 번째 키를 idx 번째 자식의 마지막 key로 입력 
    void borrowFromNext(int idx); 
  
    // idx번째 자식노드와 idx+1번째 자식노드를 합치고, 그 사이에 idx번째 키값을 넣는다. 
    void merge(int idx); 
  
    // BTree에서 class에서 내부 변수 접근 가능하게 설정.
    friend class BTree; 
}; 

// BTree
class BTree {
    BTreeNode *root; // root가 되는 BTree의 node 값 
    int degree;      // 최소 degree 값. 
public: 
  
    // 생성자 
    BTree(){
        root = NULL;
    }

    // 모든 Node 값 출력 
    void traverse(){
        if(root != NULL) root->traverse(); 
    }
  
    // 키 값이 존재하는지 검색(없을 시 NULL 리턴) 
    BTreeNode* search(int k){
        return (root == NULL)? NULL : root->search(k); 
    }
  
    // 키 값을 저장 
    void insert(int k); 

    // 키 값을 삭제 
    void remove(int k);

};

//BTreeNode의 생성자 
BTreeNode::BTreeNode(bool _leaf){
    //입력 받은 값으로 leaf 여부 세팅 
    leaf = _leaf;

    // 키 값과 자식 노드의 값을 최대로 생성한다. 
    keys = new int[DEGREE];
    C = new BTreeNode *[DEGREE+1]; 

    // 현재 저장된 값은 0으로 저장 
    n = 0;
}
  
// 키 값에 해당하는 값을 삭제
void BTreeNode::remove(int k){
    // 키보다 같거나 큰 값의 idx 값  
    int idx = 0;
    int key_val;
    
    while(idx<n && keys[idx] < k) 
        idx++;  
  
    // 키 값과 일치할 경우 수행 
    if(idx < n && keys[idx] == k){
        // leaf 노드의 여부에 따라 처리 
        if(leaf == true) {
            // 이후 값들 하나씩 당김 
            for(int i=idx+1; i<n; ++i) 
                keys[i-1] = keys[i]; 

            // 현재 노드의 키 값의 개수 하나 감소 
            n--;
        }
        else{
            if( C[idx]->n  > MIN_NODE ){
                key_val = getPrev(idx);
                keys[idx] = key_val;
                C[idx]->remove(key_val);
            }
            else if( C[idx+1]->n > MIN_NODE ){
                key_val = getNext(idx);
                keys[idx] = key_val;
                C[idx+1]->remove(key_val);
            }
            else{
                key_val = getPrev(idx);
                keys[idx] = key_val;
                C[idx]->remove(key_val);
                
                if( C[idx]->n < MIN_NODE ){
                    if( C[idx+1]->n > MIN_NODE ){
                        borrowFromNext(idx);
                    }
                    else{
                        merge(idx);
                    }
                }
            }
        }
    }
    // 키 값이 일치하지 않는 경우 
    else{
        // leaf 노드인데 큰 값이면 더 이상 찾을 수 없음 = 없는 값 
        if( leaf == true ){
            cout << k <<" 값을 찾을 수 없습니다." << endl; 
        }
        else{
            C[idx]->remove(k);
            
            if( C[idx]->n < MIN_NODE ){
                fill(idx);  
            }
        }
    }
}

// idx 번째의 자식노드 중 가장 큰 값을 가져온다. 
int BTreeNode::getPrev(int idx){
    // leaf노드를 찾을 때 까지 마지막 자식 노드를 쫓아간다. 
    BTreeNode *cur=C[idx]; 
    int n_val = cur->n;
    int ret_val;
    if(cur->leaf == false){
        ret_val = cur->C[n_val]->getPrev(n_val); 
    }
    else{
        ret_val = cur->keys[n_val];
    }

    cur->n = cur->n-1;
    // 해당 노드의 마지막 키 값을 리턴한다. 
    return ret_val;
}

// idx 번째 자식 노드 중 가장 작은 값을 가져온다. 
int BTreeNode::getNext(int idx){
    // leaf노드를 찾을 때까지  첫 번째 자식 노드를 쫓아간다. 
    BTreeNode *cur = C[idx+1]; 
    while(cur->leaf == false) 
        cur = cur->C[0]; 

    // 해당 노드의 첫 번째 키 값을 리턴한다. 
    return cur->keys[0]; 
} 
  
// idx 번째 자식 노드를 채움( idx 주변 형제노드에서 찾아서 사용, 없을 시 merge함수를 사용 )
void BTreeNode::fill(int idx){
    BTreeNode* l = C[idx-1];
    BTreeNode* r = C[idx+1];

    // idx-1 번째 키 값의 개수가 degree이상이면 idx-1 번째 자식 노드의 마지막 값을 idx 번째 자식 노드 값으로 채움 
    if( idx!=0 && l->n > MIN_NODE ){ 
        borrowFromPrev(idx);
    }
    // idx+1 번째 키 값의 개수가 degree이상이면 idx+1 번째 자식 노드의 첫 번째 값을 idx 번째 자식 노드 값으로 채운다. 
    else if( idx!=n && r->n > MIN_NODE ){
        borrowFromNext(idx);
    }
    // 형제 노드에서 가지고 올 수 없을 경우 idx 번째 노드를 idx+1 번째 노드와 합친다. 
    else{ 
        if(idx != n) 
            merge(idx);
        else
            merge(idx-1);
    } 
}
  
// idx 번째 자식 노두가 부족할 때 호출, idx-1 번째 자식의 제일 큰 키 값을 idx-1 번째 키로 idx 번째 키를 idx 번째 자식의 0번째 key로 입력 
/**
 * idx 번째 자식노드를 idx-1 번째 자식 노드의 마지막 값으로 채움, idx-1 번째 자식노드가 MIN_NODE 초과하는 값일 시 호출 
 * 1. idx번째 자식노드의 첫 번째 키 값과 첫 번재 자식 노드값의 공간을 확보한다.
 * 2. idx번째 첫 번째 키 값은 현재 노드의 idx번째 키 값으로, idx번째 첫 번째 자식노드의 값은 idx-1번째의 마지막 자식노드로 세팅 
 * 3. idx번째 키 값은 idx-1번째의 마지막 키 값으로 세팅 
 * 4. n의 개수는 알맞게 조정 
 */
void BTreeNode::borrowFromPrev(int idx){
    BTreeNode *r=C[idx]; 
    BTreeNode *l=C[idx-1]; 
    int r_cnt = r->n;
    int l_cnt = l->n;
  
    // idx 번째 자식노드의 첫 번째 키 값 확보를 위해 수행 
    for(int i=r_cnt-1; i>=0; i--) 
        r->keys[i+1] = r->keys[i]; 

    // idx 번째 자식노드의 첫 번째 키 값 세팅 
    r->keys[0] = keys[idx-1];
  
    // 자식 노드 존재 시 
    if( r->leaf==false ){
        // idx 번째 자식노드의 첫 번째 자식노드 값을 확보하기 위해 수행  
        for(int i=r_cnt; i>=0; i--) 
            r->C[i+1] = r->C[i];

        // idx-1 번째 자식 노드를 idx의 마지막 자식노드로 추가 
        r->C[0] = l->C[l->n];
    }
    
    // idx-1번째 키 값은 idx-1번째 자식 노드의 마지막 키 값으로 세팅 
    keys[idx-1] = l->keys[l_cnt-1];

    // 채워진 자식 노드 키 값의 개수는 +1, 가져온 자식 노드 키 값의 개수는 -1 
    r->n += 1;
    l->n -= 1;
} 
  
// idx 번째 자식 노드가 부족할 때 호출, idx+1 번째 자식의 제일 작은 키 값을 idx 번째 키로 idx 번째 키를 idx 번째 자식의 마지막 key로 입력
/**
 * idx+1 번째 자식노드를 idx 번째 자식 노드의 첫 번째 값으로 채움, idx+1 번째 자식노드가 MIN_NODE 초과하는 값일 시 호출
 * 1. 현재 노드의 idx 키 값은 idx 번째 마지막 키로 세팅
 * 2. idx+1 번째 노드의 첫 번째 자식 노드를 idx 번째 마지막 노드로 추가
 * 3. 현재 노드의 idx 키 값은 idx+1 번째 키 값으로 세팅
 * 4. idx+1 번째 노드를 하나 씩 왼쪽으로 이동(앞에 값 삭제 작업)
 * 5. n의 개수 최신화 
 */
void BTreeNode::borrowFromNext(int idx){
    BTreeNode *l=C[idx]; 
    BTreeNode *r=C[idx+1]; 
    int r_cnt = r->n;
    int l_cnt = l->n;

    // 현재 idx번째 키 값은 idx 번째 자식노드의 마지막 키 값으로 세팅 
    l->keys[l_cnt] = keys[idx]; 
  
    // idx+1 번째 자식노드의 첫 번째 자식노드를 idx 번째 자식 노드로 세팅 
    if(l->leaf == false)
        l->C[l_cnt+1] = r->C[0]; 
  
    // idx+1 번째 자식노드의 첫 번째 키 값은 idx 번째 키 값으로 세팅 
    keys[idx] = r->keys[0]; 
  
    // idx+1 번째 자식 노드의 키 값을 하나씩 앞으로 이동(첫 번째 값 삭제) 
    for(int i=1; i<r_cnt; ++i) 
        r->keys[i-1] = r->keys[i]; 

    // idx+1 번째 자식 노드의 자식노드를 하나씩 앞으로 이동(첫 번째 값 삭제) 
    if(r->leaf == false){
        for(int i=1; i<=r_cnt; ++i) 
            r->C[i-1] = r->C[i]; 
    } 
  
    // 채워진 자식 노드 키 값의 개수는 +1, 가져온 자식 노드 키 값의 개수는 -1 
    l->n += 1;
    r->n -= 1;
} 

/**
 * idx번째 자식노드와 idx번째 키, idx+1번째 자식 노드를 합치는 함수로, idx번째 자식과 idx+1번째 자식의 n 값이 MIN_NODE 값 이하일 때 호출된다.
 * 1. idx번째 자식노드에 현재 노드의 idx번째 키와 idx+1번째 키를 세팅 
 * 2. idx번째 자식노드에 idx+1번째 자식 노드를 세팅 
 * 3. 현재노드의 키 값과 자식 노드의 값을 이동 시킨 후 n 값을 최신화한다. 
 */
void BTreeNode::merge(int idx){
    BTreeNode *l = C[idx];
    BTreeNode *r = C[idx+1];
    int l_cnt = l->n;
    int r_cnt = r->n;
    int i;

    // idx번째 자식노드의 degree-1 번째 키 값을 idx 번째 키 값으로 세팅 
    l->keys[l_cnt] = keys[idx]; 
  
    // idx번째 자식노드의 degree 번째부터의 키 값은 idx+1 번째 자식노드의 0번째 값부터 차례대로 세팅 
    for(i=0; i<r_cnt; i++) 
        l->keys[i+l_cnt+1] = r->keys[i]; 

    // idx번째 자식노드의 degree 번째부터의 자식노드를 idx+1 번째 자식노드의 0번째 자식노드부터 차례대로 세팅 
    if( l->leaf == false ){
        for(i=0; i<=r_cnt; i++) 
            l->C[i+l_cnt+1] = r->C[i]; 
    } 

    // 합쳐진 자식노드 키 값의 개수는 현재 노드의 값과 idx+1번째 값을 더함. 현재노드 키 값의 개수는 -1
    l->n += r->n+1;
    n--;
    
    // 현재 노드의 idx이후의 키 값을 한 칸씩 앞으로 이동 
    for(i=idx+1; i<n; i++) 
        keys[i-1] = keys[i]; 

    // 현재 노드의 idx이후의 자식노드를 한 칸씩 앞으로 이동 
    for(i=idx+2; i<=n; i++) 
        C[i-1] = C[i]; 

    // 더이상 사용하지 않는 합쳐진 노드는 free시킨다. 
    delete(r);
} 
  
// 키 값을 저장한다. 
void BTree::insert(int k){
    // 생성한 이력이 없으면 root 할당 후 값 대입 
    if(root == NULL){
        root = new BTreeNode(true); 
        root->keys[0] = k;
        root->n = 1;
    }
    // root가 이미 생성되어 있을 경우 
    else{
        root->insert(k);
        
        if( root->n == DEGREE ){
            root->splitRoot();
        }
    } 
} 
  
// 현재 노드에 키를 넣을 공간이 있을 경우 호출
void BTreeNode::insert(int k){ 
    // 마지막 키를 가르치틑 idx 
    int idx = 0;
  
    // leaf일 경우 수행 
    if(leaf == true){
        // 알맞은 위치 확인 
        while(idx < n && keys[idx] < k){
            idx++;
        }
        
        // 오른 쪽 값 이동 
        for(int i=n; i>=idx; i--){
            keys[i] = keys[i-1];
        }

        // 입력받은 키 값 세팅 및 키 값의 개수 추가 
        keys[idx] = k; 
        n = n+1;
    }
    //laef가 아닐 경우 
    else{
        // 입력 받은 키 값이 들어갈 위치를 계산 
        while(idx < n && keys[idx] < k) 
            idx++;

        // 해당 자식 노드로 재귀함수 호출 
        C[idx]->insert(k);
        
        if( C[idx]->n == DEGREE ){
            splitChild(idx);
        }
    } 
} 

/**
 * 부모의 idx 번째 노드에 y노드를 분리하여 저장한다.
 * 1. y의 노드를 오른쪽 노드와 왼쪽 노드로 구분한다. 
 * 2. 부모의 노드에 idx번째 이후의 데이터를 한 칸씩 이동시킨다.
 * 3. 부모노드의 idx번째 키에 y의 가운데 값을 넣고, idx 번째 idx+1 번째 자식에 y의 좌측 우측 노드를 넣는다. 
 */
void BTreeNode::splitChild(int idx){
    // idx+1 번째 자식노드를 저장할 공간 확보
    BTreeNode *l;
    BTreeNode *r;
    int r_cnt;

    l = C[idx];
    r = new BTreeNode(l->leaf);
    r_cnt = l->n - MIN_NODE - 1;
    
    // 입력받은 node의 오른쪽 키 값을 idx+1 번째 자식 노드의 키 값에 저장 
    for(int j = 0; j < r_cnt; j++) 
        r->keys[j] = l->keys[j+MIN_NODE+1]; 
  
    // 입력받은 node의 오른쪽 자식노드를 idx+1 번째 자식 노드의 자식노드에 저장 
    if(l->leaf == false){
        for(int j = 0; j <= r_cnt; j++) 
            r->C[j] = l->C[j+MIN_NODE+1]; 
    } 
  
    // 현재 노드의 idx+1 번째 노드를 새로 할당하기위해 수행
    // idx이후의 키 값을 한 칸씩 뒤로 이동
    for(int j = n-1; j >= idx; j--)
        keys[j+1] = keys[j]; 

    if(leaf == false){
        // idx이후의 자식 노드를 한 칸씩 뒤로 이동 
        for(int j = n; j >= idx+1; j--) 
            C[j+1] = C[j];
    }

    // 입력 받은 node의 가운데 값을 idx+1번째 키 값으로 세팅 
    keys[idx] = l->keys[MIN_NODE]; 
    
    // 준비해둔 idx+1 번째 자식노드를 idx+1 번째 자식 노드에 넣는다. 
    C[idx+1] = r; 

    // 현재 노드의 키 값의 개수를 증가시킨다. 
    r->n = r_cnt;
    l->n = MIN_NODE;
    n++;
} 

/**
 * root 노드를 좌우 분리하여 저장. root의 n의 개수가 최대(DEGREE)일 때 호출 
 * 1. 현재 노드의 왼쪽 부분을 l로 옮긴다. 
 * 2. 현재 노드의 오른쪽 부분을 r로 옮긴다. 
 * 3. 현재 노드의 0번째 키에 현재 가운데 값을 넣고, n 값을 세팅한다. 
 */
void BTreeNode::splitRoot(){
    // idx+1 번째 자식노드를 저장할 공간 확보
    BTreeNode *l;
    BTreeNode *r;
    int r_cnt;
    int j;

    l = new BTreeNode(leaf);
    r = new BTreeNode(leaf);
    
    r_cnt = n - MIN_NODE - 1;
    
    for(j = 0; j < MIN_NODE; j++) 
        l->keys[j] = keys[j];

    for(j = 0; j < r_cnt; j++) 
        r->keys[j] = keys[j+MIN_NODE+1]; 

    if( leaf == false ){
        for(j = 0; j <= MIN_NODE; j++){
            l->C[j] = C[j];
        }
        for(j = 0; j <= r_cnt ; j++){
            r->C[j] = C[j+MIN_NODE+1];
        }
    }

    // 준비해둔 idx+1 번째 자식노드를 idx+1 번째 자식 노드에 넣는다. 
    keys[0] = keys[MIN_NODE];
    C[0] = l;
    C[1] = r;

    // 현재 노드의 키 값의 개수를 증가시킨다. 
    r->n = r_cnt;
    l->n = MIN_NODE;
    n = 1;
    leaf = false;
}

// 현재 노드의 아래 값을 차례대로(첫 번째 자식노드 ->  첫 번째 키 값 -> 두 번째 자식노드 ... ) 출력한다.
void BTreeNode::traverse(){
    int i; 
    for(i = 0; i < n; i++){
        if(leaf == false)
            C[i]->traverse(); 
        cout << this << " " << keys[i] << " " << endl; 
    }

    // 마지막 자식 노드 출력 
    if(leaf == false) 
        C[i]->traverse(); 
}
  
// 키 값이 존재하는지 검색(없을 시 NULL 리턴)
BTreeNode *BTreeNode::search(int k){ 
    // 키 값보다 크거나 작을 때까지 증가 
    int i = 0; 
    BTreeNode * ret_node = NULL;
     
    while(i < n && k > keys[i]) 
        i++; 

    // 키 값과 같으면 해당 객체 리턴 
    if(keys[i] == k) 
        return this; 
  
    // leaf가 아닐 경우 해당 자식노드를 대상으로 검색 수행 
    if(leaf == false) 
        ret_node = C[i]->search(k);
    
    return ret_node;
} 

// 키 값을 삭제
void BTree::remove(int k){
    // root가 생성되지 않으면 메시지 출력 
    if( root == NULL ){
        cout << "트리가 비어있습니다.\n";
    }
    else{
        // root를 기준으로 BTreeNode의 remove함수 호출 
        root->remove(k); 
      
        // 삭제 후 root의 node의 키 값의 개수가 0일 경우 첫 번째 자식 노드를 root로 설정 
        if(root->n==0){
            // 기존 루트 삭제
            delete root;
        }
    }
}

int main(){
    BTree t;
    BTreeNode *position;

    for( int i = 1; i <= 7; i++)
        t.insert(i); 

    cout << " * 초기 상태 출력" << endl;
    t.traverse(); 
    cout << endl;
    
    cout << " * 1 찾기 " << endl;
    position = t.search(1);
    if( position != NULL )
        cout << "1을 찾았습니다.";
    else
        cout << "1을 못 찾았습니다."; 
    cout << endl;
    
    cout << " * 지우기" << endl;
    t.remove(8);
    t.traverse();
    cout << endl; 
    
    
    return 0; 
}
