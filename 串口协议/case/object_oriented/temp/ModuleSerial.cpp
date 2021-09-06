#include "ModuleSerial.h"
// ------------------------------------------整数封装函数------------------------------------------
ui32* Pack(ui32  in_nums){
    char  pack_temp[5];
    ui32  pack_loop = in_nums;
    for (ui08 i = 0; i <=4; i++)
        pack_temp[i] = 0;
    for (ui08 i = 0; i < 4 && pack_loop; i++) {
        pack_temp[i] = (char) (pack_loop % 256);
        pack_loop /=256;
    }
    pack_temp[4]=0x00;
    ui32* pack_retu = (new ui32[4]());
    for (ui08 i = 0; i <=3; i++)
        pack_retu[i] = (unsigned char)pack_temp[i];
    return pack_retu;
}
// ------------------------------------------整数解包函数------------------------------------------
ui32  Undo(char in_text[]){
    ui32 undo_temp = 0;
    ui32 undo_litp = 1;
    for (ui08 i = 0; i < 4 && i<strlen(in_text); i++){
        undo_temp += (ui32)in_text[i] * (ui32)undo_litp;
        undo_litp *= 256;
    }
    return undo_temp;
}
// ------------------------------------------对象创建函数------------------------------------------
     Item::Item(const char* in_name,
                char        in_flag='G',
                char        in_type='G',
                void*       in_data= nullptr)  {
    this->Edit(in_name,
               in_flag,
               in_type,
               in_data);
}
// ------------------------------------------对象编辑函数------------------------------------------
void Item::Edit(const char* in_name,
                char        in_flag='G',
                char        in_type='G',
                void*       in_data= nullptr) {
    this->flag=in_flag;
    this->type=in_type;
    this->data=in_data;
    this->name=new char[5];
    strncpy(this->name,in_name,5);
    //this->name=(char*)in_name;
}
// ------------------------------------------对象判断函数------------------------------------------
bool Item::Ifis(const char* in_name,
                      char  in_flag = 0x00,
                      char  in_type = 0x00) const {
    bool ifis_flag = true;
    if(!in_flag && in_flag!= this->flag)ifis_flag = false;
    if(!in_type && in_type!= this->type)ifis_flag = false;
    if(!in_name && in_name!= this->name)ifis_flag = false;
    return ifis_flag;
}
// ------------------------------------------对象输出函数------------------------------------------
void* Item::Text() {
    if(this->data == nullptr)
        return (char*)"";
    ui32  text_lens = 0;
    char* text_dump = nullptr;
    char  text_temp[size];
          text_temp[0]= this->flag;
          text_temp[1]= this->type;
    // --------------扩展协议部分 Extension Protocol--------------
    if(this->type=='S'){
        text_dump = (char*)this->data;
        text_lens = strlen(text_dump);
    }
    if(this->type=='I'){
        ui32* text_ttmn = Pack(((ui32*) this->data)[0]);
        text_dump = new char[4]();
        for (ui08 i = 0; i <=3; i++) {
            text_dump[i] = (char) (text_ttmn[i] % 256);
        }
        text_lens = 4;
    }
    // ------------------End Extension Protocol-------------------
    ui32* text_lnum = Pack(text_lens);
    for (ui08 i = 0; i <=3; i++){
        text_temp[i+2] = (char)(text_lnum[i] % 256);
        text_temp[i+6] = this->name[i];
    }
    for (ui32 i = 0; i <text_lens; i++){
        text_temp[i+10] = text_dump[i];
    }
    void*  text_retu = (char*)malloc(10+text_lens);
    memcpy(text_retu,text_temp,text_lens+10);
    this->lens = text_lens+10;
    return text_retu;
}

// ------------------------------------------串口创建函数------------------------------------------
Serial::Serial(ui08 in_port,
               ui16 in_bbps = 9600,
               ui08 in_send = 0x00,
               ui08 in_dest = 0x00,
               ui16 in_nums = 0x00,
               ui16 in_time = 0x00){
    // -------外部数据初始化-------
    this->head = 0xffff;
    this->bbps = in_bbps;
    this->nums = in_nums;
    this->send = in_send;
    this->dest = in_dest;
    this->port = in_port;
    this->code = (char)0xff;
    this->time = in_time;
    // -------系统数据初始化-------
    this->lsen = 0;
    this->lrec = 0;
    this->lens = 0;
    this->tpln = 0;
    this->renm = 0;
    // -------内部数据初始化-------
    this->buff = nullptr;
    this->redt = nullptr;
    this->sedt = nullptr;
    this->flag = false;
    this->init = false;
    this->reit = new Item*;
    this->seit = new Item*;
    // -------串口数据初始化-------
    this->comInit();
    this->comLoad();

}

// ------------------------------------------输出串口列表------------------------------------------
char*Serial::comList() {
    #if dbug==true
    //输出串口信息-----------------

    //---------------------------
    #else
    //不进行此调试
    return nullptr;
    #endif
}

// ------------------------------------------配置串口信息------------------------------------------
bool Serial::comInit() {
    this->sedt = new char;
    this->sedt[0] = (char)0xff;
    this->sedt[1] = (char)0xff;
    this->sedt[1] = this->send;
    this->sedt[1] = this->dest;
    if(this->init) return false;
    else      this->init = true;
    return this->init;
}

// ------------------------------------------物理载入串口------------------------------------------
bool Serial::comLoad() {
    // 此处插入串口初始化代码-----------------------

    //------------------------------------------
    return false;
}

// ------------------------------------------修改串口信息------------------------------------------
bool Serial::comInfo(char in_send, char in_dest, bool is_init=true) {
    this->send = in_send;
    this->dest = in_dest;
    if(is_init) this->comInit();
    return this->init;
}

// ------------------------------------------检查串口状态------------------------------------------
bool Serial::comChek() {
    return this->init;
}

// ------------------------------------------执行发送程序------------------------------------------
bool Serial::comSend(){
    //------变量初始化区域----------------------------------------
    char* send_send = new char;
    char  send_tmp1[3]={0,0,0};
    char  send_text[size];
    int*  send_lb64 = nullptr;
    char* send_base = nullptr;
    //---------------------------------------------------------
    send_send[0]='\0';
    send_tmp1[0] = (char) (this->head>>4);
    send_tmp1[1] = (char) (this->head&0x00ff);
    std::strcat(send_send,send_tmp1);
    send_tmp1[0]=this->send;
    send_tmp1[1]=this->dest;
    std::strcat(send_send,send_tmp1);

    send_text[0]='\0';
    ui32* send_void = Pack(this->nums);
    for(ui08 i=0;i<=3;i++)
        send_text[i] = (char) (send_void[i]%256);
    for(ui08 i=4;i<=27;i++)
        send_text[i] = '\0';
    ui32 send_lens = 0;
    ui32 send_ldat = 0;
    for(ui08 i=0;i<this->lsen;i++){
        void* send_loop = this->seit[i]->Text();
        for(ui32 j=0;j<this->seit[i]->lens;j++){
            send_text[28+send_ldat] = ((char*)send_loop)[j];
            send_ldat++;
        }
        send_lens += this->seit[i]->lens;
    }
    send_void = Pack(send_lens);
    for(ui08 i=0;i<=3;i++)
        send_text[i+4] = (char) (send_void[i]%256);
    //此处替换为MD5计算过程-----------------------
    //md5_encode
    for(ui08 i=8;i<=27;i++){
        send_text[i] = '\0';
    }
    //----------------------------------------
    send_lb64 = new int;
    send_base = base64_encode(
            (const uint8_t *)send_text,
                         send_lens+28,
                             send_lb64);
    //printf("%s",send_base);
    for(ui32 i=0;i<=*send_lb64;i++)
        send_send[i+4] = send_base[i];
    this->lens = strlen(send_send);
    //此处替换为调用物理串口发送-----------------
    //send_send 里面是待发送的字符串
    //this->lens就是发送的字节的长度

    //----------------------------------------
    this->nums++;
    return true;
}

// ------------------------------------------执行接收程序------------------------------------------
bool Serial::comRecv(){
    char* recv_recv = new char[size];//物理接收到的字符串
    bool  recv_flag = false;         //本次是否接收到数据
    ui32  recv_uptr = 0;             //当前物理接收的位置，表示recv_recv 已经读取到第几位了
    ui32  recv_temp = 0;             //已有缓冲区的记录数，表示即将预读进入缓冲区的字节数大小
    //此处替换为调用物理串口接收-------------------
    //recv_recv里面就是接收到的字符串
    strcpy(recv_recv,(char*)"\xff\xff\1\2AAAAADIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFBTDAAAAFBJS0FIRUxMTyBXT1JMRCFQSQQAAABOVU1T5ioAAFBJBAAAAFRFTVDmKgAA\xff\xff\1\2AAAAADIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFBTDAAAAFBJS0FIRUxMTyBXT1JMRCFQSQQAAABOVU1T5ioAAFBJBAAAAFRFTVDmKgAA");
    //recv_recv = ;
    //----------------------------------------
    if(strlen(recv_recv)==0) return false;
    for(;recv_recv[recv_uptr]!='\0'&&recv_uptr<strlen(recv_recv);){
        if(!this->flag){                                            // 缓冲区为空的时候
            if(this->buff == nullptr)
                this->buff = new char[size];
            for(;recv_recv[recv_uptr]!='\xff';recv_uptr++);         // 先直接跳转读取到0xff标志位
            if(recv_recv[recv_uptr+1]=='\xff'){                     // 判断下一个的是不是也是0xff
                strncpy(this->buff,                                 // 是：把头部读取， 继续下一步
                        recv_recv+recv_uptr,
                        4);
                recv_uptr+=4;
                this->tpln = 4;
                this->flag = true;
            } else{                                                 // 否：信息不完整， 跳过此部分
                recv_uptr++;
                continue;
            }
        }
        // 此处else是为了防止只有四字头部信息ff的节头部传进来，造成内存越界，故意多循环一次强制触发检测
        else{                                                       // 缓冲区已经有数据了继续读取
            recv_temp = 0;                                          // 清空预读取的值，即开始写入
            for(;recv_recv[recv_uptr+recv_temp]!='\xff'             // 读取到下一个0xff-头部停止
              && recv_recv[recv_uptr+recv_temp]!='\0';recv_temp++); // 读取到末尾也停止，预读结束
            strncpy(this->buff+this->tpln,              // 将预读的字符串写入缓冲区内
                    recv_recv+recv_uptr,                    // 接收字符串+迭代器地址起始
                    recv_temp);                                     // 预读了多少字节的字符串长度
            recv_uptr  += recv_temp;                                // 更新实际处理的字符串的长度
            this->tpln += recv_temp;                                // 更新系统全局缓冲区占用大小
            if(recv_uptr>=strlen(recv_recv)                         // 本次是读取到了字符串的末尾
             ||recv_recv[recv_uptr]=='\0'){                         // 应该尝试是否是真的读完了的
                this->flag|=this->readStr(this->buff,this->tpln);
                return recv_flag;
            }
            if(recv_recv[recv_uptr]=='\xff'){                       // 已经看到了下一次的头部信息
                this->buff[recv_uptr]=0;
                this->flag|=this->readStr(this->buff,this->tpln);
                memset(this->buff,0,recv_uptr);
                this->tpln=0;
                this->flag=false;
            }                                                       // 此处不能return后面可能还有
        }
    }
    return recv_flag;                                               // 此处return是默认情况的返回
}

// ------------------------------------------处理串口数据------------------------------------------
bool Serial::readStr(char* in_data, ui32 in_lens) {
    ui32* read_lens = new ui32;                         // 解码获得的数据长度
    ui32  read_tlen = 0;                                // 包内真实的数据长度
    ui32  read_nums = 0;                                // 当前数据排序包编号
    ui32  read_ustr = 28;                               // 当前数据包迭代位置
    void* read_deco = malloc(size);                     // 临时解包的数据内容
    char  read_temp[size];                              // 待处理的解包的内容
    char  read_flag;
    char  read_type;
    ui32  read_dlen;
    char* read_name = nullptr;
    char* read_data = nullptr;
    Item* read_item = nullptr;                          // 临时项目的信息存储
    if(strlen(in_data)%4!=0) return false;              // 非四字节对齐，丢弃
    read_deco = base64_decode((const uint8_t *)in_data, // 调用外部函数来解包
                              in_lens,(int*)read_lens);
    for(ui32 i=0;i<*read_lens;i++)                      // 写入处理缓冲区数据
        read_temp[i]=((char*)read_deco)[i];
    read_tlen = Undo(read_temp+4);               // 获取发送的数据长度
    read_nums = Undo(read_temp);                        // 获取发送的数据包号
    if(read_tlen!=*read_lens-28) return false;          // 判断数据长度符合性
#if pnew==true
    if(read_nums< this->renm)    return false;      // 判断数据包是否最新
#endif
    // 此处是MD5校验函数--------------------------

    // -----------------------------------------
    this->renm = read_nums;
    for(;this->lrec>0; this->lrec--)                    // 清空系统原有接收区
        delete this->reit[this->lrec-1];
    if(this->redt== nullptr)
        this->redt = new char[4];
    for(ui08 i=0;i<4;i++)
        this->redt[i] = this->buff[i];
    for(;read_ustr<read_tlen+28;){
        read_flag = read_temp[read_ustr+0];
        read_type = read_temp[read_ustr+1];
        read_dlen = Undo(read_temp+2+read_ustr);
        read_name = new char [5];
        read_data = new char [read_dlen];
        read_name[4] = 0;
        for(ui08 i=0;i<4;i++)
            read_name[i] = read_temp[read_ustr+6+i];
        for(ui08 i=0;i<read_dlen;i++)
            read_data[i] = read_temp[read_ustr+10+i];
        read_item = new Item(read_name,
                             read_flag,
                             read_type,
                             read_data);
        this->reit[this->lrec] = read_item;
        this->lrec++;
        read_ustr+=read_dlen+10;
        delete [5] read_name;
        delete [read_dlen] read_data;
        read_data = nullptr;
        read_name = nullptr;
    }
    return true;
}

// ------------------------------------------清空系统缓存------------------------------------------
bool Serial::swpFree(){
    if(!this->init || this->flag) return false;
    else{
        if(!this->buff) free(this->buff);
        this->buff = new char;
        this->tpln = 0;
        return true;
    }
}

// ------------------------------------------重启串口程序------------------------------------------
bool Serial::Restart(){
    if(!this->init) return false;
    else{
        //此处是重启串口的命令-----------------------

        //---------------------------------------
    }
    return true;
}

// ------------------------------------------关闭串口程序------------------------------------------
bool Serial::Destroy(){
    if(!this->init) return false;
    else{
        //此处是关闭串口的命令-----------------------

        //---------------------------------------
    }
    return true;
}

// ------------------------------------------用户事件函数------------------------------------------
// -----------------------------------------------------------
//
//            添加发送的数据表项（发送变量和请求变量）
//
// -----------------------------------------------------------
void Serial::addSend(Item* in_data){//输入：Item*对象指针
    this->seit[this->lsen] = in_data;
    this->lsen++;
}
void Serial::addSend(const char * in_name,           //变量名称
                           char   in_flag='G',       //传输类型
                           char   in_type='G',       //数据类型
                           void*  in_data= nullptr) {//实际内容
    Item* addsend_temp = new Item(in_name,
                                  in_flag,
                                  in_type,
                                  in_data);
    this->addSend(addsend_temp);
}

// -----------------------------------------------------------
//
//        删除发送的数据表项（发送变量和请求变量）
//
//              输入：删除的是第几个0~N-1
// -----------------------------------------------------------
bool Serial::delSend(ui08 in_nums=0){
    if(in_nums>=this->lsen) return false;
    for(char i=in_nums;i<this->lsen-1;i++)
        this->seit[i]=this->seit[i+1];
    this->seit[this->lsen-1] = nullptr;
    this->lsen--;
    return true;
}

bool Serial::delSend(const char * in_name) {
    ui08 send_uptr = ptrItem(this->seit,this->lsen,in_name);
    if(send_uptr==0xff%255) return false;
    return this->delSend(send_uptr);
}

bool Serial::delSend(Item * in_uptr) {
    ui08 send_uptr = this->ptrItem(this->seit,this->lsen,in_uptr);
    if(send_uptr==0xff%255) return false;
    return this->delSend(send_uptr);
}


// --------------------------------------------------
//
//        查看发送的数据表项（发送变量和请求变量）
//
// --------------------------------------------------

char* Serial::lstName(Item ** in_item, ui08 in_lens, bool in_flag) {
    char* name_temp = new char[size];
    for(ui32 i=0;i<size;i++)
        name_temp[i]=0;
    ui08  lstsend_nums = 0;
    name_temp[0]='\0';
    for(char i=0;i<in_lens;i++){
        if(in_flag){
            std::strncat(name_temp, &in_item[i]->flag, 1);
            std::strcat(name_temp, " ");
            std::strncat(name_temp, &in_item[i]->type, 1);
            std::strcat(name_temp, " ");
        }
        std::strcat(name_temp, in_item[i]->name);
        std::strcat(name_temp, "\n");
        lstsend_nums++;
    }
    std::strcat(name_temp, "\0");
    return name_temp;
}
char* Serial::lstSend(bool in_flag=false){
    return this->lstName(this->seit,this->lsen,in_flag);
}

char* Serial::lstRecv(bool in_flag=false){
    return this->lstName(this->reit,this->lrec,in_flag);
}

// --------------------------------------------------
//
//              获取发送的数据表项
//
// --------------------------------------------------
Item* Serial::getSend(ui08 in_nums){
    if(in_nums>= this->lsen)
        return nullptr;
    else
        return this->seit[in_nums];
}

Item *Serial::getSend(const char * in_name) {
    ui08 send_uptr = ptrItem(this->seit,this->lsen,in_name);
    if(send_uptr==0xff%255)return nullptr;
    return this->getSend(send_uptr);
}

ui32 Serial::getSend_int(char in_nums) {
    if(in_nums>= this->lsen)
        return 0;
    else
        return Undo((char*)this->seit[in_nums]->data);
}

char *Serial::getSend_str(char in_nums) {
    if(in_nums>= this->lsen)
        return nullptr;
    else
        return (char*)this->seit[in_nums]->data;
}

ui32 Serial::getSend_int(const char * in_name) {
    ui08 send_uptr = ptrItem(this->seit,this->lsen,in_name);
    if(send_uptr==0xff%255)return 0;
    return Undo((char*)this->getSend(send_uptr)->data);
}

char *Serial::getSend_str(const char * in_name) {
    ui08 send_uptr = ptrItem(this->seit,this->lsen,in_name);
    if(send_uptr==0xff%255)return 0;
    return (char*)this->getSend(send_uptr)->data;
}

// --------------------------------------------------
//
//              获取接收到数据表项
//
// --------------------------------------------------
Item *Serial::getRecv(ui08 in_nums) {
    if(in_nums>=this->lrec)
        return nullptr;
    else
        return this->reit[in_nums];
}

Item *Serial::getRecv(const char * in_name) {
    ui08 recv_uptr = ptrItem(this->reit,this->lrec,in_name);
    if(recv_uptr==0xff%255)return nullptr;
    return this->getSend(recv_uptr);
}

// --------------------------------------------------
//
//              修改待发送的某个变量
//
// --------------------------------------------------
bool Serial::putSend(const char * in_name,
                           void * in_data,
                           char   in_type='\0') {
    ui08 send_uptr = ptrItem(this->seit,this->lsen,in_name);
    if(send_uptr==0xff%255) return false;
    this->seit[send_uptr]->Edit(in_name,
                                this->seit[send_uptr]->flag,
                                in_type=='\0'?this->seit[send_uptr]->type:in_type,
                                in_data);
    return false;
}

bool Serial::putSend(Item * in_data, char in_nums) {
    this->seit[in_nums] = in_data;
    return false;
}

ui32 Serial::getRecv_int(char in_nums) {
    if(in_nums >= this->lrec)
        return 0;
    else
        return Undo((char*)this->reit[in_nums]->data);
}

char *Serial::getRecv_str(char in_nums) {
    if(in_nums >= this->lrec)
        return 0;
    else
        return (char*)this->reit[in_nums]->data;
}

ui32 Serial::getRecv_int(const char * in_name) {
    ui08 recv_uptr = ptrItem(this->reit,this->lrec,in_name);
    if(recv_uptr==0xff%255)return 0;
    return Undo((char*)this->getRecv(recv_uptr)->data);
}

char *Serial::getRecv_str(const char * in_name) {
    ui08 recv_uptr = ptrItem(this->reit,this->lrec,in_name);
    if(recv_uptr==0xff%255)return 0;
    return (char*)this->getSend(recv_uptr)->data;
}
// --------------------------------------------------
//
//           通过名称或者指针查找某一个项目
//
// --------------------------------------------------
ui08 Serial::ptrItem(Item**       in_data,
                     ui08         in_nums,
                     const char * in_text) {
    for(ui08 i=0;i<in_nums;i++)
        if(strncmp(in_data[i]->name,in_text,4) == 0)
            return i;
    return 0xff%255;
}

ui08 Serial::ptrItem(Item** in_data,
                     ui08   in_nums,
                     Item * in_item) {
    for(ui08 i=0;i<in_nums;i++)
        if(in_data[i] == in_item)
            return i;
    return 0xff%255;
}













