#include "ModuleSerial.h"
#include "MSMD5Encoder.h"
#if stms==true
#include includeHead
#endif
#if enableDelay==true
	#include includeTimerHead
#endif
int buffSize = size;
// ------------------------------------------整数封装函数------------------------------------------
char *text_dump_for_int=NULL;
void*  text_retu=NULL;

ui32* pack_retu=NULL;
ui32* Pack(ui32  in_nums){
		if(pack_retu!=NULL){
				delete[] pack_retu;
				pack_retu=NULL;
		}
    char  pack_temp[5];
    ui32  pack_loop = in_nums;
		memset(pack_temp,0,5);
    for (ui08 i = 0; i < 4 && pack_loop; i++) {
        pack_temp[i] = (char) (pack_loop % 256);
        //pack_loop /=256;
				pack_loop=pack_loop>>8;
    }
    pack_temp[4]=0x00;
    pack_retu = (new ui32[4]());
    for (ui08 i = 0; i <=3; i++)
        pack_retu[i] = (unsigned char)pack_temp[i];
    return pack_retu;
}
// ------------------------------------------整数解包函数------------------------------------------
ui32  Undo(char in_text[]){
    ui32 undo_temp = 0;
    ui32 undo_litp = 1;
    for (ui08 i = 0; i < 4; i++){
        undo_temp += ((ui32)in_text[i]) * ((ui32)undo_litp);
        //undo_litp *= 256;
				undo_litp=undo_litp<<8;
    }
    return undo_temp;
}

// ------------------------------------------对象创建函数------------------------------------------
		Item::Item(){
		this->flag=this->type=this->lens=0;
		memset(this->name,0,5);
    this->data=NULL;

		}
Item::Item(const char* in_name,
                char        in_flag='G',
                char        in_type='G',
                void*       in_data= nullptr)  {
		this->lens=0;
		memset(this->name,0,5);
    this->data=NULL;
    this->Edit(in_name,
               in_flag,
               in_type,
               in_data);
}
#if stms==true								
Item::Item(const char* in_name,
                char        in_flag,
                ui32       	in_num){
	this->flag=in_flag;
	this->type='I';
	memcpy(this->name,in_name,5);
	this->theNumble=new ui32();
	*this->theNumble=in_num;
	this->data=this->theNumble;								

									
}
#endif

// ------------------------------------------对象编辑函数------------------------------------------
void Item::Edit(const char* in_name,
                char        in_flag='G',
                char        in_type='G',
                void*       in_data= nullptr) {
    this->flag=in_flag;
    this->type=in_type;									
		if(in_type=='I'){
			this->theNumble=new ui32();
			*this->theNumble=*(ui32 *)in_data;
			this->data=this->theNumble;
		}else if(in_type=='S')
			this->data=in_data;
		memcpy(this->name,in_name,5);
    //strncpy(this->name,in_name,5);
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
     //   return (char*)"";
				return nullptr;
		if(text_dump_for_int!=nullptr){
			delete[] text_dump_for_int;
			text_dump_for_int=nullptr;
		}
		if(text_retu!=NULL){
			free(text_retu);
			text_retu=NULL;
		}
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
				text_dump_for_int=new char[4]();
        text_dump =(char*)text_dump_for_int;
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
    text_retu = (char*)malloc(10+text_lens);

    memcpy(text_retu,text_temp,text_lens+10);
    this->lens = text_lens+10;
    return text_retu;
}

// ------------------------------------------串口创建函数------------------------------------------
Serial::Serial(ui08 in_port,
               ui32 in_bbps = 115200,
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
		#if enableDelay==true						 
		if(in_time<=limitDelayTime)
    this->time = in_time;
		else
		this->time = limitDelayTime;
		#else
		this->time = in_time;
		#endif
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
		#if stms==false
    this->reit = new Item*;
		#endif
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
		//USART6_Configuration(115200);
		initMySerial(this->bbps);
		return true;
    //------------------------------------------
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
    //char* send_send = new char[5];
		char* send_send;
		char temp[5];
    char  send_tmp1[3]={0,0,0};
    char  send_text[size];
    int*  send_lb64 = nullptr;
    char* send_base = nullptr;
    //---------------------------------------------------------
    //send_send[0]='\0';
    send_tmp1[0] = (char) (this->head>>4);
    send_tmp1[1] = (char) (this->head&0x00ff);
		temp[0]=send_tmp1[0];
		temp[1]=send_tmp1[1];
    //std::strcat(send_send,send_tmp1);
    send_tmp1[0]=this->send;
    send_tmp1[1]=this->dest;
		temp[2]=send_tmp1[0];
		temp[3]=send_tmp1[1];
		temp[4]=0;
    //std::strcat(send_send,send_tmp1);

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
		#if useMd5==1
		
		MD5_CTX gMd5ForSend;
		uint8_t Md5Result[16];		
		MD5Init( &gMd5ForSend );
		MD5Update( &gMd5ForSend,(unsigned char *)(send_text+28), send_lens);
		MD5Final ( Md5Result, &gMd5ForSend );
		char hmd5_extl[20];
		bool hmd5_flag=false;
		for(ui08 i=0;i<10;){
			hmd5_extl[i*2+(hmd5_flag==true?1:0)] = (hmd5_flag==true?(char)(Md5Result[i]&0x0f):(char)(Md5Result[i]>>4));
			i+=(hmd5_flag==true?1:0);
			hmd5_flag=!hmd5_flag;
		}
		for(ui08 i=8;i<=27;i++){
			if(hmd5_extl[i-8]<10)
        send_text[i] = hmd5_extl[i-8]+'0';
			else
				send_text[i] = hmd5_extl[i-8]-10+'a';
    }
		#elif useMd5==0
		for(int i=8;i<=27;i++)
			send_text[i]='0';
			
		#endif
    //----------------------------------------
    send_lb64 = new int;
		*send_lb64=4;
		/*
		base64_encode(		
            (const uint8_t *)send_text,
                         send_lens+28,
                             send_lb64,send_send);
		*/
		
    send_base = base64_encode(		
            (const uint8_t *)send_text,
                         send_lens+28,
                             send_lb64);
		


		send_send=new char[5+(*send_lb64+1)];
		for(uint8_t i=0;i<5;i++)
				send_send[i]=temp[i];
    for(ui32 i=0;i<=*send_lb64;i++)
        send_send[i+4] = send_base[i];

    //this->lens = strlen(send_send);
		this->lens=5+(*send_lb64+1);
    //此处替换为调用物理串口发送-----------------
    //send_send 里面是待发送的字符串
    //this->lens就是发送的字节的长度
		#if stms==true
			
		
		#if enableDelay==true
			//一般都不会也不建议运行这一段延时
			//这里仅保留调试用
		//while(!SerialCouldSend());
		#endif
		//从串口发送数据
		sendThePack((uint8_t*)send_send,this->lens);
		#endif
    //----------------------------------------
		delete[] send_send;
		delete send_lb64;
		send_base=nullptr;
		send_void=nullptr;
		//delete[] send_base;
    this->nums++;
		#if enableDelay==true
		this->lastTime=getNowTimeForSerial();
		#endif
    return true;
}

// ------------------------------------------执行接收程序------------------------------------------
bool Serial::comRecv(char* data, uint32_t dataLength){
    //char* recv_recv = new char[size];//物理接收到的字符串
		char recv_recv[size];
    bool  recv_flag = false;         //本次是否接收到数据
    ui32  recv_uptr = 0;             //当前物理接收的位置，表示recv_recv 已经读取到第几位了
    ui32  recv_temp = 0;             //已有缓冲区的记录数，表示即将预读进入缓冲区的字节数大小
    //此处替换为调用物理串口接收-------------------
    //recv_recv里面就是接收到的字符串
    //strcpy(recv_recv,(char*)"\xff\xff\1\2AAAAADIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFBTDAAAAFBJS0FIRUxMTyBXT1JMRCFQSQQAAABOVU1T5ioAAFBJBAAAAFRFTVDmKgAA\xff\xff\1\2AAAAADIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFBTDAAAAFBJS0FIRUxMTyBXT1JMRCFQSQQAAABOVU1T5ioAAFBJBAAAAFRFTVDmKgAA");
		memset(recv_recv,0,size);
		//for(uint32_t i=0;i<dataLength;i++)
		//	recv_recv[i]=data[i];
		dataLength=(dataLength>(size-1)?(size-1):dataLength);
		memcpy(recv_recv,data,dataLength);
		recv_recv[dataLength]='\0';
    //----------------------------------------

		//for(int t=0;t<dataLength;t++)
			//printf("%c",recv_recv[t]);
	//	printf("\0\r\n");
		
    if(dataLength==0) return false;						
    for(;recv_recv[recv_uptr]!='\0'&&recv_uptr<dataLength;){
        if(!this->flag){                                            // 缓冲区为空的时候
            if(this->buff == nullptr)
							 //this->buff = new char[size];
								this->buff = new char[buffSize];
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
					
            if(this->tpln+recv_temp<=buffSize)
						strncpy(this->buff+this->tpln,              // 将预读的字符串写入缓冲区内
                    recv_recv+recv_uptr,                    // 接收字符串+迭代器地址起始
                    recv_temp);                                     // 预读了多少字节的字符串长度
						else{
							this->flag = false;
							this->tpln = 0;
							memset(this->buff,0,this->tpln);	
							continue;
						}
						
            recv_uptr  += recv_temp;                                // 更新实际处理的字符串的长度
            this->tpln += recv_temp;                                // 更新系统全局缓冲区占用大小
            if(recv_uptr>=dataLength                         // 本次是读取到了字符串的末尾
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
		//delete[] recv_recv;
    return recv_flag;                                               // 此处return是默认情况的返回
}

// ------------------------------------------处理串口数据------------------------------------------
bool Serial::readStr(char* in_data, ui32 in_lens) {
    ui32* read_lens = new ui32;                         // 解码获得的数据长度
    ui32  read_tlen = 0;                                // 包内真实的数据长度
    ui32  read_nums = 0;                                // 当前数据排序包编号
    ui32  read_ustr = 28;                               // 当前数据包迭代位置
    //void* read_deco = malloc(size);                     // 临时解包的数据内容
		char* read_deco;             								        // 临时解包的数据内容
    char  read_temp[size];                              // 待处理的解包的内容
    char  read_flag;
    char  read_type;
    ui32  read_dlen;
    //char* read_name = nullptr;
		char read_name[5];
    //char* read_data = nullptr;
		char read_data[size];
    Item* read_item = nullptr;                          // 临时项目的信息存储
    if(strlen(in_data)%4!=0){
			delete read_lens;
			return false;              // 非四字节对齐，丢弃
		}


    read_deco = base64_decode((const uint8_t *)in_data, // 调用外部函数来解包
                              in_lens,(int*)read_lens);

    for(ui32 i=0;i<*read_lens;i++)                      // 写入处理缓冲区数据
        read_temp[i]=((char*)read_deco)[i];
    read_tlen = Undo(read_temp+4);               // 获取发送的数据长度
    read_nums = Undo(read_temp);                        // 获取发送的数据包号
    if(read_tlen!=*read_lens-28){
				delete read_lens;
				return false;          // 判断数据长度符合性
		}



#if pnew==true
    if(read_nums< this->renm)    return false;      // 判断数据包是否最新
#endif
    // 此处是MD5校验函数--------------------------
/*
		MD5_CTX gMd5ForRecv;
		uint8_t Md5Result[16];		
		MD5Init( &gMd5ForRecv );
		MD5Update( &gMd5ForRecv,(unsigned char *)(read_temp+28), read_tlen );
		MD5Final ( Md5Result, &gMd5ForRecv );		
		char hmd5_extl[20];
		bool hmd5_flag=false;
		for(ui08 i=0;i<10;){
			hmd5_extl[i*2+(hmd5_flag==true?1:0)] = (hmd5_flag==true?(char)(Md5Result[i]&0x0f):(char)(Md5Result[i]>>4));
			i+=(hmd5_flag==true?1:0);
			hmd5_flag=!hmd5_flag;
		}
		
		for(ui08 i = 0;i<20;i++)
		if(hmd5_extl[i]<10){
			if(hmd5_extl[i]!=read_temp[8+i]-'0'){
				delete read_lens;
				return false;
			}			
		}else{
			if(hmd5_extl[i]!=read_temp[8+i]+10-'a'){
				delete read_lens;
				return false;
			}				
		}*/



					//返回前记得delete
    // -----------------------------------------
    this->renm = read_nums;
    for(;this->lrec>0; this->lrec--){
				// 清空系统原有接收区
				//delete[] (char *)this->reit[this->lrec-1]->data;
				//delete[] this->reit[this->lrec-1]->name;
				this->reitPlus[this->lrec-1].deleteItemForProc();
		}

    if(this->redt== nullptr)
        this->redt = new char[4];
    for(ui08 i=0;i<4;i++)
        this->redt[i] = this->buff[i];
		
    for(;read_ustr<read_tlen+28;){
        read_flag = read_temp[read_ustr+0];
        read_type = read_temp[read_ustr+1];
        read_dlen = Undo(read_temp+2+read_ustr);
        //read_data = new char [read_dlen];
        read_name[4] = 0;
        for(ui08 i=0;i<4;i++)
            read_name[i] = read_temp[read_ustr+6+i];
        for(ui08 i=0;i<read_dlen;i++)
            read_data[i] = read_temp[read_ustr+10+i];
				
				this->reitPlus[this->lrec].renewItemForProc(read_flag,read_type,read_name,read_data,read_dlen);
															
        this->lrec++;
        read_ustr+=read_dlen+10;
    }
		/*
		if(read_data!=nullptr){
			 delete[] read_data;
			read_data = nullptr;	
		}*/

	
		delete read_lens;
		read_deco=nullptr;
    return true;
}

// ------------------------------------------清空系统缓存------------------------------------------
bool Serial::swpFree(){
    if(!this->init || this->flag) return false;
    else{
        //if(!this->buff) free(this->buff);
				//if(!this->buff) delete[] this->buff;
        //this->buff = new char;
				if(this->buff!=nullptr){
					delete[] this->buff;
					this->buff=nullptr;
				}
        this->tpln = 0;
        return true;
    }
}

// ------------------------------------------重启串口程序------------------------------------------
bool Serial::Restart(){
    if(!this->init) return false;
    else{
        //此处是重启串口的命令-----------------------
				//USART_Cmd(communicationUart,ENABLE);
			  USART_ITConfig(communicationUart,USART_IT_RXNE,ENABLE);
				USART_Cmd(communicationUart,ENABLE);
        //---------------------------------------
    }
    return true;
}

// ------------------------------------------关闭串口程序------------------------------------------
bool Serial::Destroy(){
    if(!this->init) return false;
    else{
        //此处是关闭串口的命令-----------------------
				//USART_Cmd(communicationUart,DISABLE);
				USART_ITConfig(communicationUart,USART_IT_RXNE,DISABLE);
				USART_Cmd(communicationUart,DISABLE);
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
void Serial::addSend(const char * in_name,           //变量名称
                           char   in_flag='G',       //传输类型
                           ui32  	in_data=0) {//实际内容
    Item* addsend_temp = new Item(in_name,
                                  in_flag,
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
    if(in_nums>=this->lsen||in_nums<0) return false;
			//memset(this->seit[in_nums]->name,0,5);
			if(this->seit[in_nums]->type=='I'){
			delete this->seit[in_nums]->theNumble;
			this->seit[in_nums]->theNumble=nullptr;			
			}		
    for(char i=in_nums;i<this->lsen-1;i++){
			/*
			#if stms==false
			delete this->seit[i]->name;
			#else
			memset(this->seit[i]->name,0,5);
			if(this->seit[i]->type=='I'){
			delete this->seit[i]->theNumble;
			this->seit[i]->theNumble=nullptr;			
			}
			#endif
			delete this->seit[i];			
			
			
			this->seit[i] = new Item(this->seit[i+1]->name,
                                  this->seit[i+1]->flag,
                                  this->seit[i+1]->type,
                                  this->seit[i+1]->data);
			
			*/
			this->seit[i]=this->seit[i+1];
		}

		
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
void  Serial::delAllSend(){
	for(int i=0;i<this->lsen;i++){
		memset(this->seit[i]->name,0,5);
		if(this->seit[i]->type=='I'){
			delete this->seit[i]->theNumble;
			this->seit[i]->theNumble=nullptr;			
		}
		//delete this->seit[this->lsen-1]->data;
		delete this->seit[i];
	
    this->seit[i] = nullptr;		
	}
	this->lsen=0;
		
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
#if stms==false
char* Serial::lstRecv(bool in_flag=false){
    return this->lstName(this->reit,this->lrec,in_flag);
}
#endif
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
ItemPlus Serial::getRecv(ui08 in_nums) {
    if(in_nums>=this->lrec){
			ItemPlus tempItemP;
			return tempItemP;
		}
    else
        return this->reitPlus[in_nums];
}

ItemPlus Serial::getRecv(const char * in_name) {
    ui08 recv_uptr = ptrItemPlus(this->reitPlus,this->lrec,in_name);
    if(recv_uptr==0xff%255){
			ItemPlus tempItemP;
			return tempItemP;
		}
    //return this->getSend(recv_uptr);
		return this->reitPlus[recv_uptr];
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

ui32 Serial::getRecv_int(int in_nums) {
    if(in_nums >= this->lrec)
        return 0;
    else
        return Undo((char*)this->reitPlus[in_nums].data);
}

char *Serial::getRecv_str(int in_nums) {
    if(in_nums >= this->lrec)
        return 0;
    else
        return (char*)this->reitPlus[in_nums].data;
}
int Serial::getRecv_str_length(char in_nums) {
    if(in_nums >= this->lrec||strcmp((char *)this->reitPlus[in_nums].type,"P")==0)
        return -1;
    else
        return strlen((char*)this->reitPlus[in_nums].data);
}
uint8_t Serial::getRecv_num(){
	return this->lrec;
}
bool Serial::revNumIsvaild(uint8_t in_nums){
	if(in_nums>=this->lrec)
		return false;
	else
		return true;
}

ui32 Serial::getRecv_int(const char * in_name) {
		#if stms==true
    ui08 recv_uptr = ptrItemPlus(this->reitPlus,this->lrec,in_name);
		#else
		ui08 recv_uptr = ptrItem(this->reit,this->lrec,in_name);
		#endif
    if(recv_uptr==0xff%255)return 0;
		#if stms==true
        return Undo((char*)this->reitPlus[recv_uptr].data);
		#else
				return Undo((char*)this->reit[recv_uptr]->data);
		#endif		
}

char *Serial::getRecv_str(const char * in_name) {
		#if stms==true
    ui08 recv_uptr = ptrItemPlus(this->reitPlus,this->lrec,in_name);
		#else
		ui08 recv_uptr = ptrItem(this->reit,this->lrec,in_name);
		#endif
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

ui08 ptrItemPlus(ItemPlus*       in_data,
                     ui08         in_nums,
                     const char * in_text) {
    for(ui08 i=0;i<in_nums;i++)
        if(strncmp(in_data[i].name,in_text,4) == 0)
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

ui08 toSend(ui08 in_id){
    ui08 ex_id = 0x00;
    ex_id |= ( in_id & 0x80) >> 4;						//10000000b
    ex_id |= ( in_id & 0x40) >> 6;						//01000000b
    ex_id |= ( in_id & 0x20) >> 4;						//00100000b
    ex_id += ((in_id & 0x10) >> 4) *  3;			//00010000b
    ex_id += ( in_id & 0x08) >> 1;						//00001000b
    ex_id += ((in_id & 0x04) >> 2) *  5;			//00000100b
    ex_id += ((in_id & 0x02) >> 1) *  6;			//00000010b
    ex_id += ((in_id & 0x01) >> 0) *  7;			//00000001b
    return ex_id;
}

ui08 toRecv(ui08 in_id){
    ui08 ex_id = 0x00;
    ex_id |= (in_id & 0x80) << 7;				//00001000b
    ex_id |= 0X01 << (~in_id & 0x07);		//00000111b
    return ex_id;
}


#if stms==true
/*
bool Serial::testcomRecv(char* tt,uint8_t tt2){
	  char* recv_recv = new char[size];//物理接收到的字符串
    bool  recv_flag = false;         //本次是否接收到数据
    ui32  recv_uptr = 0;             //当前物理接收的位置，表示recv_recv 已经读取到第几位了
    ui32  recv_temp = 0;             //已有缓冲区的记录数，表示即将预读进入缓冲区的字节数大小
    //此处替换为调用物理串口接收-------------------
    //recv_recv里面就是接收到的字符串
    strncpy(recv_recv,tt,tt2);
		//USART6_sendChar('1');
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
		//this->getRecv_str((char)0);
		//USART6_Print((uint8_t *)this->getRecv_str((char)0),3);
		
		delete[] recv_recv;
    return recv_flag;                                               // 此处return是默认情况的返回
	
}
*/

ItemPlus::ItemPlus(){
//	this->data=nullptr;
		memset(this->name,0,5);
		memset(this->data,0,5);
}
void 	ItemPlus::renewItemForProc(ui08 in_flag,ui08 in_type,char *in_name,char *in_dat,int length){
	
	this->flag=in_flag;
	this->type=in_type;
	memcpy(this->name,in_name,5);
	/*
	if(this->data!=nullptr){
		delete[] this->data;
	}
	if(this->type=='I')
		this->data=new char[length];
	else{
		this->data=new char[length+1];
		this->data[length]=0;
	}*/
	memcpy(this->data,in_dat,length);
	if(this->type=='S')
		this->data[length]=0;
}
void 	ItemPlus::deleteItemForProc(){
	memset(this->name,0,5);
	/*
	if(this->data!=nullptr){
		delete[] this->data;
		this->data=nullptr;
	}*/
}

bool 	Serial::changeData(uint8_t in_nums,char* strData){
	//修改字符串类型数据
	if(in_nums>=this->lsen||this->seit[in_nums]->type!='S')
		return false;
	this->seit[in_nums]->data=strData;
	return true;
}
bool  Serial::changeData(uint8_t in_nums,uint32_t numData){
	//修改整数类型数据
	if(in_nums>=this->lsen||this->seit[in_nums]->type!='I')
		return false;
	*this->seit[in_nums]->theNumble=numData;
	return true;

}

bool  Serial::changeSendNum(uint8_t newSendNum){
	//修改发送方编号
	this->send=newSendNum;
	return true;	
	
}
bool  Serial::changeRevNum(uint8_t newRevNum){
	//修改接收方编号
	this->dest=newRevNum;
	return true;	
	
}
bool  Serial::changeFlag(uint8_t in_nums,char newFlag ){
	//修改某一个元素的传播方式（获取或接收）
	if(in_nums>=this->lsen||(!(newFlag=='G'||newFlag=='P')))
		return false;
	this->seit[in_nums]->flag=newFlag;
	return true;
	
}

bool  Serial::changeName(uint8_t in_nums,char* newName){
	//修改某一个元素的名称
	if(in_nums>=this->lsen)
		return false;
	if(strlen(newName)>5)
		return false;
	memset(this->seit[in_nums]->name,0,5);
	memcpy(this->seit[in_nums]->name,newName,5);
	return true;
}

bool 	Serial::changeTypeToStr(uint8_t in_nums,char* strData){
		if(in_nums>=this->lsen)
		return false;
		if(this->seit[in_nums]->type=='S')
			this->changeData(in_nums,strData);
		else{
			delete this->seit[in_nums]->theNumble;
			this->seit[in_nums]->theNumble=nullptr;
			this->seit[in_nums]->type='S';
			Serial::changeData(in_nums,strData);
		}
		return true;
}
bool  Serial::changeTypeToInt(uint8_t in_nums,uint32_t numData){
		if(in_nums>=this->lsen)
		return false;
		if(this->seit[in_nums]->type=='I')
			this->changeData(in_nums,numData);
		else{
			this->seit[in_nums]->theNumble=new ui32();
			*this->seit[in_nums]->theNumble=numData;
			this->seit[in_nums]->data=this->seit[in_nums]->theNumble;
			this->seit[in_nums]->type='I';
		}	
		return true;
}
char Serial::getRecvFlag(uint8_t in_nums){
    if(in_nums >= this->lrec)
        return 0;
    else
        return (char)this->reitPlus[in_nums].flag;

}	
char Serial::getRecvType(uint8_t in_nums){
	  if(in_nums >= this->lrec)
        return 0;
    else
        return (char)this->reitPlus[in_nums].type;
}
char* Serial::getRecvName(uint8_t in_nums){
		  if(in_nums >= this->lrec)
        return nullptr;
    else
        return (char *)this->reitPlus[in_nums].name;
}
ui32 Serial::getRecvRenm(){
	//获取发送方的序列号
	return this->renm;
}

uint8_t Serial::isLatestPack(){
	if(this->getRecvRenm()>=this->ackNum&&this->getRecvRenm()>0){
		this->ackNum=this->getRecvRenm()+1;
		return 1;
	}else
		return 0;	
}
void Serial::renewACK(){
	this->ackNum=this->getRecvRenm()+1;
}

#if enableDelay==true
bool Serial::SerialCouldSend(){
	uint32_t nowTime=getNowTimeForSerial();
	uint32_t countWaitingTime=(nowTime>this->lastTime)?(nowTime-this->lastTime):(MaxTimeCount-this->lastTime+nowTime);
	if(countWaitingTime>=this->time)
		return true;
	else
		return false;
}
#endif
#endif






