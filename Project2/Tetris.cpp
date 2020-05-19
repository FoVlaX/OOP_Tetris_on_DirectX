#include "Tetris.h"

int block::globalid = 0;
block* block::ids[1000] = { NULL };

Tetris::Tetris(HINSTANCE h)
{
	hinstance = h;
	width = 9;
	height = 23;
	OBJECT::setH(height); //��������� ������ ���� ��� ����������� ��������
	Init(); //�������������� ���� � ����. ����������
}

Tetris::Tetris(int X, int Y, HINSTANCE h)
{
	hinstance = h;
	width = X;
	height = Y;
	
	OBJECT::setH(height);//��������� ������ ���� ��� ����������� ��������
	Init();//�������������� ���� � ����. ����������
}

void Tetris::Run()
{
	MSG msg;
	d3d->ViewDist = 45;
	figure* f = (figure*)new squareFigure(width / 2,0, height, width);
	blockDeleter clearBlocks(width,height);

	OBJECT* GameOver = NULL;
	bool PulseUp = true;
	int figureMoving = 0;
	do {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			if (GameOver == NULL) {
				if (figureMoving == 0) {
					if (GetAsyncKeyState(VK_LEFT))
					{
						f->moveLeft();
					}
					if (GetAsyncKeyState(VK_UP))
					{
						f->rotate();
					}
					if (GetAsyncKeyState(VK_RIGHT))
					{
						f->moveRight();
					}

					if (!f->moveDown()) {
						delete f;
						int r = rand() % 4;
						switch (r) {
						case 0: f = (figure*)new squareFigure(width / 2, 0, height, width); break;
						case 1: f = (figure*)new lineFigure(width / 2, 0, height, width); break;
						case 2: f = (figure*)new GFigure(width / 2, 0, height, width); break;
						case 3: f = (figure*)new GFigure(width / 2, 0, height, width); break;
						default:
							break;
						}
						if (!f->moveDown()) {
							GameOver = CreatObjGameOver(); // ������� ������ ������ � ������� ��������� ������ ���� GameOver
						}
						clearBlocks.checkOnDeletion();
					}

				}

				figureMoving++;
				if (figureMoving > 10) figureMoving = 0;
			} else {
				
				///////////////////// ��������� ���� Game Over
				if (PulseUp && GameOver->xs < 7) {
					GameOver->xs *= 1.01;
					GameOver->ys *= 1.01;
					GameOver->zs *= 1.01;
				}
				else {
					if (PulseUp && GameOver->xs >= 7)  PulseUp = false;
				}


				if (!PulseUp && GameOver->xs > 4) {
					GameOver->xs /= 1.01;
					GameOver->ys /= 1.01;
					GameOver->zs /= 1.01;
				}
				else {
					if (!PulseUp && GameOver->xs <= 4)  PulseUp = true;

				}
				/////////////////
				if (GetAsyncKeyState(VK_SHIFT)) { //������� ���� ������� ����� GameOver � ������� ������, ���� ������� ��� ���� �� ������ � ���� ����
					delete GameOver;
					GameOver = NULL;
					clearBlocks.deleteAllBlocks();
					delete f;
					int r = rand() % 4;
					switch (r) {
					case 0: f = (figure*)new squareFigure(width / 2, 0, height, width); break;
					case 1: f = (figure*)new lineFigure(width / 2, 0, height, width); break;
					case 2: f = (figure*)new GFigure(width / 2, 0, height, width); break;
					case 3: f = (figure*)new GFigure(width / 2, 0, height, width); break;
					default:
						break;
					}
				}
			}
			d3d->Render(); //�������� �����
		}

	} while (msg.message != WM_QUIT);//���� ������� ������ �������� ���� ����� �� �����
}

Tetris::~Tetris()
{
	delete d3d;
	delete win;
	delete w1;
	delete w2;
	delete f;
}

bool Tetris::Init() //�������������
{
	win = new WININIT(WINDOW_WIDTH, WINDOW_HEIGHT, "The Tetris", hinstance, hwnd);
	if (hwnd == NULL)
	{
		return false;
	}
	d3d = new D3DINIT(hwnd, hinstance);
	if (!d3d->Initialization()) {
		return 0;
	}
	d3d->SetViewPoint(10, 20, 4);
	d3d->SetGameSpeed(60);
	
	light = new LIGHT({ -0.5f,1.4f,-0.6f,1.f }, { 1.f,1.f,1.f,1.f }, normalLight, { 0.f,0.f,0.f,0.f });
	 f = new plane(width*2,{1.f,1.f,1.f,1.f}, {width/2,height+1}); //������� ����� �������� � ������ ����� ������� ���������
	 w1 = new Wall(height, { 0.f,1.f,0.f,1.f }, { -1, height / 2 + 1 });
	 w2 = new Wall(height, { 0.f,1.f,0.f,1.f }, { width, height / 2 + 1 });
	return true;
}

OBJECT* Tetris::CreatObjGameOver()
{
	HRESULT hr;
	OBJECT* GameOver = new OBJECT("gameover.obj", "startex.dds", hr);
	GameOver->z = -4;
	GameOver->x = 10;
	GameOver->y = 20;
	GameOver->blend = { 1.f,0.1f,0.8f,1.f };
	GameOver->xs = 4;
	GameOver->ys = 4;
	GameOver->zs = 4;
	GameOver->yang = 3 * XM_PIDIV2;
	return GameOver;
}
