#pragma once
#include "Common.h"

// 默认窗口参数
static const WCHAR DEFAULTWINDOWCLASSNAME[] = L"MindEye";
static const WCHAR DEFAULTWINDOWNAME[] = L"MindEye";

// 声音渲染顺序
static const DWORD PSMAINMIX = 255;

// 默认声音参数
static const DWORD DEFAULTCHANNELS = 2;
static const DWORD DEFAULTSAMPLERATE = 48000;
static const DWORD DEFAULTBLOCKALIGN = 8;
static const DWORD DEFAULTBYTERATE = DEFAULTSAMPLERATE * DEFAULTBLOCKALIGN;

// 自定义消息
static const DWORD MY_PAINT = 0x0800;
static const DWORD MY_REFRESH = 0x0900;
static const DWORD MY_QUIT = 0x0C00;

// 绘制用参数
static const DWORD MODE_DEFAULT = 0x00000000;
static const DWORD MODE_SOLID = 0x00000001;

// 以撒的结合对象类地址偏移
static const int OBJFLG1OFFSET = 0x0028;
static const int OBJFLG2OFFSET = 0x002C;
static const int XPOSOFFSET = 0x0294;
static const int YPOSOFFSET = 0x0298;

// 对象标签
static const DWORD CHAROBJFLG = 0x0001;
static const DWORD TEAROBJFLG = 0x0002;
static const DWORD FAMIOBJFLG = 0x0003;
static const DWORD BOMBOBJFLG = 0x0004;
static const DWORD PKUPOBJFLG = 0x0005;
static const DWORD MACHOBJFLG = 0x0006;
static const DWORD BARROBJFLG = 0x0009;
static const DWORD SHKPOBJFLG = 0x0011;
static const DWORD SHLYOBJFLG = 0x03E8;

// 对象边框颜色
static const D2D1::ColorF CHARFRAMECLR(0.0f, 1.0f, 0.0f, 1.0f);
static const D2D1::ColorF TEARFRAMECLR(0.2f, 1.0f, 0.2f, 1.0f);
static const D2D1::ColorF FAMIFRAMECLR(1.0f, 1.0f, 0.0f, 1.0f);
static const D2D1::ColorF BOMBFRAMECLR(0.5f, 0.0f, 0.0f, 1.0f);
static const D2D1::ColorF PKUPFRAMECLR(0.0f, 0.0f, 1.0f, 1.0f);
static const D2D1::ColorF MACHFRAMECLR(0.0f, 0.0f, 0.5f, 1.0f);
static const D2D1::ColorF BARRFRAMECLR(1.0f, 0.2f, 0.2f, 1.0f);
static const D2D1::ColorF SHKPFRAMECLR(0.0f, 0.0f, 0.5f, 1.0f);
static const D2D1::ColorF ENEMFRAMECLR(1.0f, 0.0f, 0.0f, 1.0f);
static const D2D1::ColorF SHLYFRAMECLR(0.0f, 1.0f, 0.0f, 1.0f);

// 对象边框大小
static const DWORD CHARFRAMESIZE = 40;
static const DWORD TEARFRAMESIZE = 20;
static const DWORD FAMIFRAMESIZE = 20;
static const DWORD BOMBFRAMESIZE = 30;
static const DWORD PKUPFRAMESIZE = 40;
static const DWORD MACHFRAMESIZE = 40;
static const DWORD BARRFRAMESIZE = 20;
static const DWORD SHKPFRAMESIZE = 40;
static const DWORD ENEMFRAMESIZE = 40;
static const DWORD SHLYFRAMESIZE = 40;

// 人物专用
static const int CHMAXHPOFFSET = 0x1290;
static const int CHHPOFFSET = 0x1294;
static const int CHHHPOFFSET = 0x1298;
static const int CHSHEILDOFFSET = 0x129C;
static const int CHSHEILDFLGOFFSET = 0x12A0;
static const int CHKEYOFFSET = 0x12AC;
static const int CHBOMBOFFSET = 0x12B4;
static const int CHMONEYOFFSET = 0x12B8;
static const int CHCHIDOFFSET = 0x130C;

// 墙壁边框颜色
static const D2D1::ColorF WALLNONECLR(0.0f, 0.0f, 0.0f, 1.0f);
static const D2D1::ColorF WALLNORMCLR(0.1f, 0.1f, 0.1f, 1.0f);
static const D2D1::ColorF WALLDOORCLR(0.0f, 1.0f, 0.0f, 1.0f);

// 地形标签
static const DWORD TERRNORM = 0x0000;
static const DWORD TERRRAVI = 0x0001;
static const DWORD TERROBST = 0x0003;
static const DWORD TERRWALL = 0x0004;
static const DWORD TERRDOOR = 0x0005;

// 地形特性
static const DWORD TERRPRONORMAL = 0x0000;
static const DWORD TERRPROSPIKE = 0x03E7;
static const DWORD TERRPROOBSTAC = 0x03E8;
static const DWORD TERRPRORAVINE = 0x0BB8;

// 地形边框颜色
static const D2D1::ColorF TERRNORMCLR(0.0f, 0.0f, 0.0f, 1.0f);
static const D2D1::ColorF TERRSPIKCLR(1.0f, 0.0f, 0.0f, 1.0f);
static const D2D1::ColorF TERRRAVICLR(0.0f, 0.0f, 1.0f, 1.0f);
static const D2D1::ColorF TERROBSTCLR(0.3f, 0.0f, 0.0f, 1.0f);
static const D2D1::ColorF TERRWALLCLR(0.05f, 0.05f, 0.05f, 1.0f);
static const D2D1::ColorF TERRDOORCLR(0.0f, 1.0f, 0.0f, 1.0f);

// 地形边框大小
static const DWORD TERRFRAMESIZE = 30;
