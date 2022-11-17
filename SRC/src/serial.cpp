#include "../inc/serial.hpp"

//CRC校验表
static const unsigned int crc8Table[256] =
{

    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,

    0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,

    0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,

    0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,

    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,

    0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,

    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,

    0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,

    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,

    0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,

    0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,

    0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,

    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,

    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,

    0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,

    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,

};

Uart::Uart(){
	
}

Uart::~Uart(){
	close(fd);
}

//CRC校验
unsigned char calcCRC(unsigned char* data,unsigned int len)
{
	
   unsigned  char crc8 = 0;
    while (len--)
    {
        crc8 = crc8 ^ (*data++);
        crc8 = crc8Table[crc8];
    }
	//printf("crc8Table:  %hhu \n",crc8Table[crc8]);
	//cout<<"crc8Table:  "<<crc8Table[crc8]<<endl;
    return crc8;

}

void Uart::Close(){
	close(fd);
}
	
bool Uart::Open(const char * device, int _speed, int _parity, bool _should_block,int data_length) {
	uart_path = device;
	fd = open(uart_path, O_RDWR | O_NOCTTY | O_SYNC|O_NONBLOCK);
	speed = _speed;
	parity = _parity;
	should_block = _should_block;

        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
               std::printf("getattr error\n");
		return false;
        }
        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays

	tcflush( fd , TCIOFLUSH );
	tty.c_cc[VMIN]  = should_block ?  data_length : 0;// read doesn't block?	TODO 已修改
        tty.c_cc[VTIME] = 0.5;			// 0.5 seconds read timeout TODO 已修改

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;		//TODO 使用硬件流控制

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
               	printf("setattr error\n");
		return false;
        }
        //cout << "open serial successful" << endl;
	return true;
}

bool Uart::restart(){
	Close();
	fd = open(uart_path, O_RDWR | O_NOCTTY | O_SYNC);
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0)
	{
		    printf("getattr error\n");
		    return false;
	}
	cfsetospeed (&tty, speed);
	cfsetispeed (&tty, speed);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tty.c_iflag &= ~IGNBRK;         // disable break processing
	tty.c_lflag = 0;                // no signaling chars, no echo,
					    // no canonical processing
	tty.c_oflag = 0;                // no remapping, no delays

	tcflush( fd , TCIOFLUSH );

	tty.c_cc[VMIN]  = should_block ? 7 : 0;// read doesn't block?
	tty.c_cc[VTIME] = 1;			// 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
					    // enable reading
	tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr (fd, TCSANOW, &tty) != 0)
	{
	      printf("setattr error\n");
		    return false;
	}
	return true;
	  
}

bool Uart::TestR(unsigned char a[])
{
	int readsize = read( fd , a , 16 );
	tcflush( fd , TCIFLUSH );
	
	if (readsize != 16) {
		//cout << "READ ERROR:" << readsize << endl;
		return false;
	}
	union FloatToCharUnion {
		int num;
		unsigned char data[4];
	}_FloatToCharUnion;
	int op=1;
	_FloatToCharUnion.data[0] = a[0];
	_FloatToCharUnion.data[1] = a[1];
	_FloatToCharUnion.data[2] = a[2];
	_FloatToCharUnion.data[3] = a[3];
  	for(int i=0;i<4;i++)
  	//cout<<(int)a[i]<<"  ";
  	//cout<<endl;
	op = _FloatToCharUnion.num;
	
	//cout<<(int)op<<endl;
	return true;
}


//third in 4 out 5
bool Uart::ReadMode(unsigned char data[16],unsigned char &id,unsigned char &mode){
	int readsize = read( fd , data , 16 );
	tcflush( fd , TCIFLUSH );
	if (readsize != 16) {
		//cout << "READ ERROR:" << readsize << endl;
//		printf("read size\n");
		return false;
	}
	if(data[0] != 0x2d){
		//cout<< "READ ERROR FAULT"<<endl;
//		printf("head\n");
		return false;
	}
	id = data[1];
	mode = data[2];
//	printf("data2 = %hhu\n", mode);
	unsigned char check = 0;
	for (int inc = 0; inc < 15; inc++){
		check ^= data[inc];
	}
	
	//printf("id = %hhu, mode = %hhu\n", id, mode);
	return true;
/*	if(check != data[15]){
		printf("3\n");
		return false;
	}else{
		return true;
	}
*/
}
bool Uart::ReadData(unsigned char data[16],unsigned char &mode,atomic<int>&pitch_now,atomic<int>&yaw_now,atomic<int>&shoot_vel){
	int readsize = read( fd , data , 16 );
	tcflush( fd , TCIFLUSH );
	if (readsize != 16) {
		ze++;
		cout << "READ ERROR:" << readsize << endl;
		return false;
	}
	//cout<<"data15:  "<<data[15]<<endl;
	//cout<<"CRC:  "<<(int)calcCRC(data,16)<<endl;
	//printf("data0:  %hhu \n",data[0]);
	//printf("CRC:  %hhu \n",calcCRC(data,16));
	mutex m_mutex;
	int base_pitch=pitch_now;
	int base_yaw=yaw_now;
	int base_shoot_vel=shoot_vel;
	//cout<<base_pitch<<endl;
	
	union FloatToCharUnion {
		int num;
		unsigned char data[4];
	}_FloatToCharUnion;
    //cout<<data[0]<<endl;
	// id first , mode second
	//mode = (unsigned char)data[1];
	mode = (unsigned char)data[2];
	//unsigned char id=data[2];
	//id=data[2];
	id=data[1];	
	//pitch 
	_FloatToCharUnion.data[3] = data[3];
	_FloatToCharUnion.data[2] = data[4];
	_FloatToCharUnion.data[1] = data[5];
	_FloatToCharUnion.data[0] = data[6];
	//printf("data6:  %hhu \n",data[6]);
	//cout<<(int)data[3]<<"  "<<(int)data[4]<<"  "<<(int)data[5]<<"   "<<(int)data[6]<<endl;
	pitch_now = _FloatToCharUnion.num;
	//cout<<"reception pitch "<<_FloatToCharUnion.num<<endl;
	//yaw
	_FloatToCharUnion.data[3] = data[7];
	_FloatToCharUnion.data[2] = data[8];
	_FloatToCharUnion.data[1] =data[9];
	_FloatToCharUnion.data[0] =data[10];
	yaw_now = _FloatToCharUnion.num;
	//cout<<(int)data[7]<<"  "<<(int)data[8]<<"  "<<(int)data[9]<<"   "<<(int)data[10]<<endl;
	/*if(yaw_now>100000|yaw_now<-100000)
	{
		_FloatToCharUnion.data[3] = data[7];
		_FloatToCharUnion.data[2] = data[8];
		_FloatToCharUnion.data[1] = data[9];
		_FloatToCharUnion.data[0] = data[10];
		yaw_now =_FloatToCharUnion.num;
	}
	if(yaw_now>100000|yaw_now<-100000)
	{
		yaw_now=base_yaw;
	}*/
	_FloatToCharUnion.data[3] = data[11];
	_FloatToCharUnion.data[2] = data[12];
	_FloatToCharUnion.data[1] = data[13];
	_FloatToCharUnion.data[0] = data[14];
	shoot_vel = _FloatToCharUnion.num;
	
	unsigned char check = 0;
	for (int inc = 0; inc < 15; inc++){
		check ^= data[inc];
	}
	//cout<<"check:"<<(int)check<<"   "<<(int)data[15]<<"     "<<(int)data[0]<<endl;
	if(data[0]!=0x2d)
	{
		pitch_now=base_pitch;
		yaw_now=base_yaw;
		shoot_vel=base_shoot_vel;
		//cout<<"read false"<<endl;
	}
	/*else
	{
		for(int t=7;t<11;t++)
			cout<<(int)data[t]<<"     ";
		//cout<<endl;
	}*/
	//cout<<"data0:  "<<data[0]<<endl;
	if (data[0] == 0x2d){
		cout<<"     222read successful"<<endl;
		success_++;
		return true;
	}
	else 
	{
		/*if(data[0] == 0x2d){
			success_++;			
		}else{
			fail_++;
		}*/
		cout<<"   333read fail"<<endl;
		printf("data0:  %hhu \n",data[0]);
		fail_++;
		return false;
	}
}

bool Uart::SendData(unsigned char* data, int len){
	if( write ( fd , data , len ) != len ){
		//cout<<write ( fd , data , len )<<endl;
		
		//cout<<len<<endl;
		tcflush( fd , TCOFLUSH );
		
		//printf("uart send error\n");
		return false;   // send 22 character greeting
	}
	//usleep ((len) * 100);
	//for (int inc = 0; inc < 28; inc++){
	//	cout << inc << ":"<< (int)(unsigned char)data[inc] <<"  " ;
	//}
	//cout << endl;
	tcflush( fd , TCOFLUSH );
	return true;
	
}


bool Uart::readId(unsigned char data[1],unsigned char& id)
{
  int readsize = read( fd , data , 1 );
	tcflush( fd , TCIFLUSH );
	
	if (readsize != 1) {
		//cout << "READ ERROR:" << readsize << endl;
		return false;
	}
	id = (unsigned char)data[0];
	
}

bool Uart::TestSend(int a)
{
	 unsigned char data[4];
	union IntToCharUnion {
		int num;
		unsigned char data[4];
	}_IntToCharUnion;
	_IntToCharUnion.num=a;
	data[0]=_IntToCharUnion.data[0];
	data[1]=_IntToCharUnion.data[1];
	data[2]=_IntToCharUnion.data[2];
	data[3]=_IntToCharUnion.data[3];
	if(SendData(data, 4) == true)
	{
	  //cout<<"send successful"<<endl;
	  return true;
	}
	else{
	  //cout<<"send failed"<<endl;
	  return false;
	}
}


bool Uart::sendToMCU(unsigned char fire, short target_pitch, short target_yaw, short distance)
{
	 unsigned char data[10];
	union IntToCharUnion {
		int num;
		unsigned char data[4];
	}_IntToCharUnion;
	
	union op{
		short a;
		unsigned char data[2];
	}_op;
	
// 	data[0]=0x3d;
// 	data[1]=fire;
//  	_op.a=target_pitch;
// //	cout<<"serial "<<target_pitch<<endl;
// // 	//cout<<_IntToCharUnion.num<<endl;
//  	data[2]=_op.data[0];
//  	data[3]=_op.data[1];
// 	_op.a=target_yaw;
//  	data[4]=_op.data[0];
//  	data[5]=_op.data[1];
	
//  	_op.a = distance;
//  	data[6]=_op.data[0];
//  	data[7]=_op.data[1];


//  	data[8]=calcCRC(data,8);
//  	//cout<<(int)data[8]<<endl;
//  	data[9]=0;
	data[0] = 0x3d;
	data[1] = fire;
	_op.a=target_pitch;
	//cout<<"serial "<<target_pitch<<endl;
	//cout<<_IntToCharUnion.num<<endl;
	data[2]=_op.data[0];
	data[3]=_op.data[1];
	_op.a=target_yaw;
	data[4]=_op.data[0];
	data[5]=_op.data[1];
	
	_op.a = distance;
	data[6]=_op.data[0];
	data[7]=_op.data[1];
	data[8]= 0;
	data[9]= 0;
	if(SendData(data, 10) == true)
	{
	  //cout<<"send successful"<<endl;
     		printf("Serial send successful\n");
	  return true;
	}
	else
{
      		printf("fail\n");
	  //cout<<"send failed"<<endl;
	  return false;
	}
}




