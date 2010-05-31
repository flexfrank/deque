#include <ruby/ruby.h>
static VALUE deque_class;
typedef struct deque_struct{
  VALUE first;
  VALUE last;
} deque_t;

static void 
deque_mark(void* deque){
  deque_t* d=deque;
  rb_gc_mark(d->first);
  rb_gc_mark(d->last);
}

static void
deque_free(void* deque){
  deque_t* d=deque;
  d->first=Qnil;
  d->last=Qnil;
  free(d);
}

#define INIT_SIZE 64
static void
deque_init(deque_t*deque){
  deque->first=rb_ary_new();
  deque->last=rb_ary_new();
}

/*static void
deque_init_with_array(deque_t* d,VALUE ary){
  d->first=rb_ary_new();
  d->last=rb_ary_dup(ary);
}*/

static void
deque_init_with_arrays(deque_t* d,VALUE first,VALUE last){
  d->first=rb_ary_dup(first);
  d->last=rb_ary_dup(last);
}

/*static VALUE
rdeque_init(VALUE self){
  deque_t* d;
  Data_Get_Struct(self,deque_t, d);
  deque_init(d);
  return Qnil;
}*/

static VALUE
deque_debug(deque_t* d){
  VALUE result=rb_ary_new();

  rb_ary_push(result,d->first);
  rb_ary_push(result,d->last);
  return result;
}

static VALUE
rdeque_debug(VALUE self){
  deque_t* d;
  Data_Get_Struct(self,deque_t, d);
  return deque_debug(d);
}


static VALUE
deque_alloc(VALUE klass){
  deque_t* deque = ALLOC(deque_t);
  deque_init(deque);
  return Data_Wrap_Struct(klass,deque_mark , deque_free, deque);

}

static VALUE deque_with_arrays(VALUE first,VALUE last){
  deque_t* deque = ALLOC(deque_t);
  deque_init_with_arrays(deque,first,last);
  return Data_Wrap_Struct(deque_class,deque_mark , deque_free, deque);
}


static void
deque_add_first(deque_t* d,VALUE obj){
  rb_ary_push(d->first,obj);
}

static void
deque_add_last(deque_t* d,VALUE obj){
  rb_ary_push(d->last,obj);
}

static void
deque_balance_first(deque_t* d){

  size_t llen=RARRAY_LEN(d->last);
  size_t next_first_size=llen/2;
  size_t next_last_size=llen-next_first_size;
  d->first=rb_ary_reverse(rb_ary_subseq(d->last,0,next_first_size));

  d->last=rb_ary_subseq(d->last,next_first_size,next_last_size);

}
static void
deque_balance_last(deque_t* d){
  size_t flen=RARRAY_LEN(d->first);
  size_t next_last_size=flen/2;
  size_t next_first_size=flen-next_last_size;
  
  d->last=rb_ary_reverse(rb_ary_subseq(d->first,0,next_last_size));

  d->first=rb_ary_subseq(d->first,next_last_size,next_first_size);

}


static VALUE
deque_remove_first(deque_t* d){
  size_t flen=RARRAY_LEN(d->first);
  size_t llen=RARRAY_LEN(d->last);
  if(flen== 0){
    if(llen==0){
      return Qnil;
    }else if(llen==1){
      return rb_ary_pop(d->last);
    }else if( llen> 1){
      deque_balance_first(d);
    }

  }
    

  return rb_ary_pop(d->first);
}

static VALUE
deque_remove_last(deque_t* d){
  size_t flen=RARRAY_LEN(d->first);
  size_t llen=RARRAY_LEN(d->last);
  if(llen==0){

    if(flen==0){
      return Qnil;
    }else if(flen==1){
      return rb_ary_pop(d->first);
    }else if(flen>1){
      deque_balance_last(d);
    }
  }

  return rb_ary_pop(d->last);
}

static VALUE
rdeque_add_last(VALUE self,VALUE obj){
  deque_t* d;
  Data_Get_Struct(self,deque_t, d);
  deque_add_last(d,obj);
  return self;
}

static VALUE
rdeque_add_first(VALUE self,VALUE obj){
  deque_t* d;
  Data_Get_Struct(self,deque_t, d);
  deque_add_first(d,obj);
  return self;
}

static VALUE
rdeque_remove_first(VALUE self){
  deque_t* d;
  Data_Get_Struct(self,deque_t, d);
  return deque_remove_first(d);
}

static VALUE
rdeque_remove_last(VALUE self){
  deque_t* d;
  Data_Get_Struct(self,deque_t, d);
  return deque_remove_last(d);
}

static size_t
deque_size(deque_t* d){
  return RARRAY_LEN(d->first)+RARRAY_LEN(d->last);
}
static VALUE
rdeque_size(VALUE self){
  deque_t* d;
  Data_Get_Struct(self,deque_t,d);
  return LONG2NUM(deque_size(d));
}

static size_t deque_index(deque_t* d,size_t i){
  size_t firstlen=RARRAY_LEN(d->first);
  if(i<firstlen){
    return firstlen-1-i;
  }else{
    return i-firstlen;
  }
}
static VALUE deque_target_array(deque_t* d, size_t i){
  if(i<RARRAY_LEN(d->first)){
    return d->first;
  }else{
    return d->last;
  }
}

static VALUE deque_at(deque_t* d, size_t i){
  VALUE ary=deque_target_array(d,i);
  size_t index=deque_index(d,i);
  return rb_ary_entry(ary,index);
}

static VALUE deque_entry(deque_t* d,long i){
  if(i<0){
    i+=deque_size(d);
  }
  return deque_at(d,i);
}

static VALUE rdeque_at(VALUE self,VALUE index){
  long i=NUM2LONG(index);
  deque_t* d;

  Data_Get_Struct(self,deque_t, d);

  return deque_entry(d,i);
}

static void deque_store_at(deque_t* d, size_t i, VALUE obj){
  VALUE ary=deque_target_array(d,i);
  size_t index=deque_index(d,i);
  rb_ary_store(ary,index,obj);
}
static void deque_store(deque_t* d, long i, VALUE obj){
  if(i<0){i+=deque_size(d);}
  deque_store_at(d,i,obj);
}

static VALUE rdeque_store(VALUE self,VALUE index,VALUE obj){
  long i=NUM2LONG(index);
  deque_t* d;
  Data_Get_Struct(self,deque_t,d);
  deque_store(d,i,obj);
  return obj;

}

static VALUE rdeque_each(VALUE self){
  deque_t* d;
  RETURN_ENUMERATOR(self, 0, 0);

  Data_Get_Struct(self,deque_t,d);

  {long i;
    for(i=RARRAY_LEN(d->first)-1;i>=0;i--){
      rb_yield(RARRAY_PTR(d->first)[i]);
      if(RARRAY_LEN(d->first)<i){
        i=RARRAY_LEN(d->first);
      }
    }
  }
  
  {size_t i;
    for(i=0;i<RARRAY_LEN(d->last);i++){
      rb_yield(RARRAY_PTR(d->last)[i]);
    }
  }
  return self;
}

static void deque_clear(deque_t* d){
  rb_ary_clear(d->first);
  rb_ary_clear(d->last);
}

static VALUE rdeque_clear(VALUE self){
  deque_t* d;
  Data_Get_Struct(self,deque_t,d);
  deque_clear(d);
  return self;
}

static void
deque_first_range(deque_t* d, size_t beg,size_t len,size_t* rbeg,size_t* rlen){
  long first_len=RARRAY_LEN(d->first);
  if(beg<first_len){
    if(beg+len-1<first_len){
      *rbeg=first_len-beg-len;
      *rlen=len;
    }else{
      *rbeg=0;
      *rlen=first_len-beg;
    }
  }else{
    *rbeg=0;
    *rlen=0;
  }
 
}

static void
deque_last_range(deque_t* d, size_t beg,size_t len,size_t* rbeg,size_t* rlen){
  long first_len=RARRAY_LEN(d->first);
  if(first_len < beg+len){
    if(first_len<=beg){
      *rbeg=beg-first_len;
      *rlen=len;
    }else{
      *rbeg=0;
      *rlen=beg+len-first_len;
    }
  }else{
    *rbeg=0;
    *rlen=0;
  }

}

static VALUE deque_first_slice(deque_t* d, size_t beg, size_t len){
  size_t rbeg,rlen;
  if(len==0)return rb_ary_new();
    
  deque_first_range(d,beg,len,&rbeg,&rlen);
  if(rlen>0){
    return rb_ary_subseq(d->first,rbeg,rlen);
  
  }else{
    return rb_ary_new();
  }
}

static VALUE deque_last_slice(deque_t* d, size_t beg, size_t len){
  size_t rbeg,rlen;
  if(len==0)return rb_ary_new();
  deque_last_range(d,beg,len,&rbeg,&rlen);
  if(rlen>0){
    return rb_ary_subseq(d->last,rbeg,rlen);
  }else{
    return rb_ary_new();
  }

}

static VALUE deque_subseq(deque_t* d,size_t beg,size_t len){
  VALUE first,last;
  if(beg>deque_size(d)) return Qnil;
  if(beg <0 || len < 0) return Qnil;
  first=deque_first_slice(d,beg,len);
  last=deque_last_slice(d,beg,len);
  return deque_with_arrays(first,last);
}

static VALUE deque_range_set(deque_t* d,size_t beg,size_t len,VALUE obj){
  size_t fbeg,flen,lbeg,llen;
  ID setid=rb_intern("[]=");

  deque_first_range(d,beg,len,&fbeg,&flen);
  deque_last_range(d,beg,len,&lbeg,&llen);
  VALUE argsf[3]={LONG2NUM(fbeg),LONG2NUM(flen),obj};

  if(flen>0 && llen>0){
    rb_funcall2(d->first,setid,3,argsf);
    d->last=rb_ary_subseq(d->last,llen,RARRAY_LEN(d->last)-llen);
    return obj;
  }
  if(flen>0){
    rb_funcall2(d->first,setid,3,argsf);
    return obj;
  }
  if(llen>0){
    VALUE argsl[3]={LONG2NUM(lbeg),LONG2NUM(llen),obj};
    rb_funcall2(d->last,setid,3,argsl);
    return obj;
  }

  return obj;
}

static VALUE rdeque_aset(int argc,VALUE* argv,VALUE self){
  long beg,len;
  deque_t* d;
  Data_Get_Struct(self,deque_t,d);

  if(argc==3){
    beg=NUM2LONG(argv[0]);
    len=NUM2LONG(argv[1]);
    deque_range_set(d,beg,len,argv[2]);
    return argv[2];
  }

  if(argc!=2){
    rb_raise(rb_eArgError,"wrong number of arguments (%d for 2)",argc);
  }
  if(FIXNUM_P(argv[0])){
    deque_store(d,FIX2LONG(argv[0]),argv[1]);
    return argv[1];
  }
  if(rb_range_beg_len(argv[0],&beg,&len,deque_size(d),0)){
    deque_range_set(d,beg,len,argv[1]);
    return argv[1];
  }
  deque_store(d,NUM2LONG(argv[0]),argv[1]);
  return argv[1];
}

static VALUE rdeque_aref(int argc, VALUE* argv, VALUE self){
  deque_t* d;
  VALUE arg;
  Data_Get_Struct(self,deque_t,d);
  long beg,len;

  if(argc==2){
    beg=NUM2LONG(argv[0]);
    len=NUM2LONG(argv[1]);
    if(beg<0){
      beg+=deque_size(d);
    }
    return deque_subseq(d,beg,len);
  }

  if(argc !=1){
    rb_scan_args(argc,argv,"11",0,0);
  }
  arg=argv[0];
  
  if(FIXNUM_P(arg)){
    return deque_entry(d,FIX2LONG(arg));
  }
  /* check if idx is Range */
  switch (rb_range_beg_len(arg, &beg, &len, deque_size(d), 0)) {
    case Qfalse:
      break;
    case Qnil:
      return Qnil;
    default:
      return deque_subseq(d, beg, len);
  }
  return deque_entry(d, NUM2LONG(arg));

}

static VALUE
deque_hash_recursive(VALUE obj,VALUE arg,int rec){
  deque_t* d;
  long h;

  Data_Get_Struct(obj,deque_t,d);

  h=NUM2LONG(rb_hash(deque_class));
  if(rec){
    h+=rb_hash(deque_class);
  }else{
    long i,c=0;
    for(i=RARRAY_LEN(d->first)-1;i>=0;i--){
      h+=NUM2LONG(rb_hash(RARRAY_PTR(d->first)[i]))*c;
      c++;
    }
    for(i=0;i<RARRAY_LEN(d->last);i++){
      h+=NUM2LONG(rb_hash(RARRAY_PTR(d->last)[i]))*c;
      c++;
    }
  }
  return LONG2FIX(h);

}

static VALUE
rdeque_hash(VALUE self)
{
  return rb_exec_recursive(deque_hash_recursive,self,Qnil);
}

static VALUE
rdeque_dup(VALUE self){
  deque_t* d;
  Data_Get_Struct(self,deque_t,d);
  return deque_with_arrays(d->first,d->last);
}

static VALUE
deque_to_a(deque_t* d){
  VALUE r=rb_ary_reverse(rb_ary_dup(d->first));
  rb_ary_concat(r,d->last);
  return r;
}

static VALUE
rdeque_to_a(VALUE self){
  deque_t* d;
  Data_Get_Struct(self,deque_t,d);
  return deque_to_a(d);
}

static VALUE
deque_equal_rec(VALUE deq1,VALUE deq2,int rec){
  long i;

  if(rec) return Qtrue;
  deque_t *d1,*d2;
  Data_Get_Struct(deq1,deque_t,d1);
  Data_Get_Struct(deq1,deque_t,d2);
  for(i=0;i<deque_size(d1) && i<deque_size(d2);i++){
    if(!rb_equal(deque_at(d1,i),deque_at(d2,i))){
      return Qfalse;
    }
  }
  return deque_size(d1)==deque_size(d2);
}

static VALUE
rdeque_equal(VALUE self,VALUE another){
  if(self==another) return Qtrue;
  if(deque_class==rb_obj_class(another)){
    deque_t *d,*a;
    Data_Get_Struct(self,deque_t,d);
    Data_Get_Struct(another,deque_t,a);
    if(deque_size(d)==deque_size(a)){
      return rb_exec_recursive_paired(deque_equal_rec,self, another, another);
    }else{
      return Qfalse;
    }
  }else{
    return Qfalse;
  }
}

static VALUE
deque_first(deque_t* d){
  long flen=RARRAY_LEN(d->first);
  if(flen>0){
    return RARRAY_PTR(d->first)[flen-1];
  }else{
    if(RARRAY_LEN(d->last)>0){
      return RARRAY_PTR(d->last)[0];
    }else{
      return Qnil;
    }
  }
}

static VALUE
deque_last(deque_t* d){
  long llen=RARRAY_LEN(d->last);
  if(llen>0){
    return RARRAY_PTR(d->last)[llen-1];
  }else{
    if(RARRAY_LEN(d->first)>0){
      return RARRAY_PTR(d->first)[0];
    }else{
      return Qnil;
    }
  }
}


static VALUE
rdeque_first(int argc,VALUE* argv,VALUE self){
  deque_t* d;
  Data_Get_Struct(self,deque_t, d);
  if(argc==0){
    return deque_first(d);
  }else if(argc==1){
    long len=NUM2LONG(argv[0]);
    if(len>=0){
      return deque_subseq(d,0,len);
    }
    rb_raise(rb_eArgError,"negative size");
  }else{
    rb_raise(rb_eArgError,"wrong number of arguments (%d for 0..1)",argc);
  }
}

static VALUE
rdeque_last(int argc,VALUE* argv,VALUE self){
  deque_t* d;
  Data_Get_Struct(self,deque_t, d);
  if(argc==0){
    return deque_last(d);
  }else if(argc==1){
    long len=NUM2LONG(argv[0]);
    if(len>=0){
      return deque_subseq(d,deque_size(d)-len,len);
    }
    rb_raise(rb_eArgError,"negative size");
  }else{
    rb_raise(rb_eArgError,"wrong number of arguments (%d for 0..1)",argc);
  }
}

static void
deque_concat(deque_t* d,VALUE ary){
  rb_ary_concat(d->last,ary);
}

static VALUE
rdeque_concat(VALUE self,VALUE ary){
  deque_t *d;
  Data_Get_Struct(self,deque_t,d);
  if(rb_obj_class(ary)==deque_class){
    deque_t* d2;
    Data_Get_Struct(ary,deque_t,d2);
    ary=deque_to_a(d2);
  }
  deque_concat(d,ary);
  return self;
}

static VALUE rdeque_is_empty(VALUE self){
  deque_t* d;
  Data_Get_Struct(self,deque_t,d);
  return deque_size(d)==0 ? Qtrue : Qfalse;
}
void
Init_deque_core(void){
  VALUE dequeClass=rb_define_class("Deque",rb_cObject);
  deque_class=dequeClass;
  rb_define_alloc_func(dequeClass, deque_alloc);
  //rb_define_private_method(dequeClass, "initialize", rdeque_init, 0);
  rb_define_method(dequeClass, "add_last", rdeque_add_last, 1);
  rb_define_method(dequeClass, "add_first", rdeque_add_first, 1);
  rb_define_method(dequeClass, "remove_first", rdeque_remove_first, 0);
  rb_define_method(dequeClass, "remove_last", rdeque_remove_last, 0);
  rb_define_method(dequeClass, "size", rdeque_size, 0);
  rb_define_method(dequeClass, "empty?", rdeque_is_empty, 0);
  rb_define_method(dequeClass, "length", rdeque_size, 0);
  rb_define_method(dequeClass, "debug", rdeque_debug, 0);
  rb_define_method(dequeClass, "at", rdeque_at, 1);
  rb_define_method(dequeClass, "store",rdeque_store,2);
  rb_define_method(dequeClass, "each",rdeque_each,0);
  rb_define_method(dequeClass, "clear",rdeque_clear,0);
  rb_define_method(dequeClass, "[]",rdeque_aref,-1);
  rb_define_method(dequeClass, "[]=",rdeque_aset,-1);
  rb_define_method(dequeClass, "hash",rdeque_hash,0);
  rb_define_method(dequeClass, "dup",rdeque_dup,0);
  rb_define_method(dequeClass, "to_a",rdeque_to_a,0);
  rb_define_method(dequeClass, "==",rdeque_equal,1);
  rb_define_method(dequeClass, "concat",rdeque_concat,1);
  rb_define_method(dequeClass, "first",rdeque_first,-1);
  rb_define_method(dequeClass, "last",rdeque_last,-1);

}
