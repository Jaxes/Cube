#pragma once
#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "Camera.h"
#include <math.h>
#include <stack>
#include<windows.h>
#include <mmsystem.h>
#include<dsound.h>
#pragma comment(lib, "WINMM.LIB")

class CubeApp : public D3DApp
{
public:
	CubeApp(HINSTANCE hInstance);
	~CubeApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	//定义几何形状
	void BuildShapeGeometryBuffers();
	//误差纠正函数
	void CheckError(XMFLOAT4X4& m);
private:
	//几何形状的顶点缓冲区+索引缓冲区
	ID3D11Buffer * mShapesVB;
	ID3D11Buffer* mShapesIB;
	
	ID3D11ShaderResourceView* mTexSRV;
	//6个方向光(消除由于灯光带来的亮度不均)
	DirectionalLight mDirLights[6];

	//材料（光）属性
	Material mBoxMat;

	//定义从本地空间到世界空间的转换。
	XMFLOAT4X4 mBoxWorld[27];

	int mBoxVertexOffset;

	UINT mBoxIndexOffset;

	UINT mBoxIndexCount;

	Camera mCam;

	bool PressKey;
	bool wasPress;
	bool isDone;
	bool isMess;
	bool PressSpace;

	POINT mLastMousePos;

	XMFLOAT3 m_target;
	XMFLOAT3 m_up;

	enum KEY{
		Q_KEY, W_KEY, E_KEY,
		R_KEY, T_KEY, Y_KEY,
		U_KEY, I_KEY, O_KEY,
		A_KEY, S_KEY, D_KEY,
		F_KEY, G_KEY, H_KEY,
		J_KEY, K_KEY, L_KEY,
		PUP_KEY, PDOWN_KEY,
		UP_KEY, DOWN_KEY,
		LEFT_KEY, RIGHT_KEY,NONE
	} Key;

	std::stack<KEY> s;

	//记录打乱次数，每完成一次减一
	int MessStep;
	//记录旋转次数
	int t;
};