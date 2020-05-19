#pragma once
#include "D3DINIT.h"
#include "WININIT.h"
#include <stdlib.h>
#include <time.h>

struct Coord{ //coordinates в игровой плоскости
	int x;
	int y;
};

class block;
class plane;
class Wall;

class Tetris
{
public:
	Tetris(HINSTANCE h);
	Tetris(int X, int Y, HINSTANCE h);
	void Run();
	~Tetris();
protected:
	HWND hwnd; // уникальный идентификатор окна HWND.
	WININIT* win;//   объект окна              (WINDOW_WIDTH, WINDOW_HEIGHT, "The Tetris", hinstance, hwnd)
	D3DINIT* d3d;//   объект отвечающий за графику DirectX11             (hwnd);
	HINSTANCE hinstance;
	LIGHT* light; // нормальный свет
	int height; // высота игрового поля
	int width; // ширина игрового поля
	bool Init(); //инициализация окна и граф библиотеки
	plane *f;
	Wall *w1;
	Wall *w2;
	OBJECT* CreatObjGameOver();
};


class block {
public:
	static int globalid;
	static block *ids[1000];
	block() {
		id = globalid;
		ids[globalid++] = this;
		HRESULT hr = S_OK;
		o  = new OBJECT("test.obj", "startex.dds", hr);
		o->blend = { 1.f,1.f,1.f,1.f };
		Pos = { 0, 0 };
		o->setCoord(0, 0);
	}
	block(XMFLOAT3 size) {
		id = globalid;
		ids[globalid++] = this;
		HRESULT hr = S_OK;
		o = new OBJECT("test.obj", "startex.dds", hr);
		o->blend = { 1.f,1.f,1.f,1.f };
		o->setCoord(0, 0);
		Pos = { 0, 0 };
		o->xs = size.x;
		o->ys = size.y;
		o->zs = size.z;
	}
	block(XMFLOAT4 Color) {
		id = globalid;
		ids[globalid++] = this;
		HRESULT hr = S_OK;
		o = new OBJECT("test.obj", "startex.dds", hr);
		o->blend = Color;
		Pos = { 0, 0 };
		o->setCoord(0, 0);
	}
	block(XMFLOAT4 Color, Coord pos) {
		id = globalid;
		ids[globalid++] = this;
		HRESULT hr = S_OK;
		o = new OBJECT("test.obj", "startex.dds", hr);
		o->blend = Color;
		o->setCoord(pos.x, pos.y);
		Pos = pos;
	}

	block(XMFLOAT4 Color, Coord pos, XMFLOAT3 size) {
		id = globalid;
		ids[globalid++] = this;
		HRESULT hr = S_OK;
		o = new OBJECT("test.obj", "startex.dds", hr);
		o->blend = Color;
		o->setCoord(pos.x, pos.y);
		o->xs = size.x;
		o->ys = size.y;
		o->zs = size.z;
		Pos =pos;
	}
	void SetSize(XMFLOAT3 size) {
		o->xs = size.x;
		o->ys = size.y;
		o->zs = size.z;
	}
	void setCoord(Coord pos) {
		o->setCoord(pos.x, pos.y);
		Pos = pos;
	}
	void SetColor(XMFLOAT4 color) {
		o->blend = color;
	}
	~block() {
		for (int i = id; i < globalid - 1; i++) {
			ids[i] = ids[i + 1];
			ids[i]->id--;
		}
		ids[--globalid] = NULL;
		delete o;
	}

	block *Collision(float X, float Y,int height, int width) { //проверка на столкнование с границами и другими блоками в координатах X Y
		if (X < 0 || X >= width || Y > height) {
			return this;
		}
		for (int i = 0; i < globalid; i++) {
			if (ids[i]->id != id) {
				if (abs(ids[i]->Pos.x - X)<1.0 && abs(ids[i]->Pos.y - Y)<1.0) {
					return ids[i];
				}
			}
		}
		return NULL;
	}
	Coord getPos() {
		return Pos;
	}
protected:
	OBJECT* o;
	Coord Pos;
	int id;
};


class plane : public block {//площадка наслeдует класc блок и задает ему размер H на H с высотой в 1
public:
	plane(float H) : block({ 20.f,1.f,20.f }) { 

	}
	plane(float H,XMFLOAT4 Color) : block({ H,1.f,H }) {
		o->blend = Color;
	}
	plane(float H,XMFLOAT4 Color, Coord pos) : block(Color, pos, { H,1.f,H }) {

	}
};

class Wall : public block { //класс границы наследует блок и задает уме размер 1 на 1 с высотой H
public:
	Wall(float H) : block({ 1.f,H,1.f }) {

	}
	Wall(float H,XMFLOAT4 Color) : block({ 1.f,H,1.f }) {
		o->blend = Color;
	}
	Wall(float H,XMFLOAT4 Color, Coord pos) : block(Color, pos, { 1.f,H,1.f }) {

	}
};


struct figForRotate {
	int X;
	int Y;
	Coord blocksPos[4];
};


class figure { //фигура класс который будут наследовать фигуры разных типов
public:
	figure(float x, float y, float h, float w) {
		X = x;
		Y = y;
		height = h;
		width = w;
	}
	
	~figure() {

	}//виртуальные методы
	virtual bool moveDown() = 0; //движение вниз если возвращает правду движение возможно, иначе фигура приземлилась
	virtual void moveLeft() = 0; //движение влево
	virtual void moveRight() = 0; //движение вправо
	virtual void rotate() = 0; //вращение фигуры
protected:
	float X;
	float Y;
	float height;
	float width;
	block *blocks[4];
};

class squareFigure : public figure {
public:
	squareFigure(int x, int y,int h, int w) : figure(x, y, h, w) {
		XMFLOAT4 color = { 1.0f,0.0f,0.0f,1.0f };
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				blocks[i*2+j] = new block(color, { x+i,y+j });
			}
		}
	}
	bool moveDown() {
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.y+=1;
			block* c = blocks[i]->Collision(p.x, p.y, height, width);
			if (c == blocks[i]) {
				return false;
			}
			if (c != NULL) {
				bool p = false;
				for (int j = 0; j < 4; j++) {
					if (c == blocks[j]) {
						p = true;
						break;
					}
				}
				if (!p){
					return false;
				}
			}
		}
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.y+=1;
			blocks[i]->setCoord(p);
		}
		Y++;
		return true;
	}
	void moveLeft() {
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.x--;
			block* c = blocks[i]->Collision(p.x, p.y, height, width);
			if (c == blocks[i]) {
				return;
			}
			if (c != NULL) {
				bool p = false;
				for (int j = 0; j < 4; j++) {
					if (c == blocks[j]) {
						p = true;
						break;
					}
				}
				if (!p) {
					return;
				}
			}
		}
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.x--;
			blocks[i]->setCoord(p);
		}
		X--;
	}
	void moveRight() {
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.x++;
			block* c = blocks[i]->Collision(p.x, p.y, height, width);
			if (c == blocks[i]) {
				return;
			}
			if (c != NULL) {
				bool p = false;
				for (int j = 0; j < 4; j++) {
					if (c == blocks[j]) {
						p = true;
						break;
					}
				}
				if (!p) {
					return;
				}
			}
		}
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.x++;
			blocks[i]->setCoord(p);
		}
		X++;
	} //движение вправо
	void rotate() {
	} //вращение фигуры для квадрата вращения нет
};


class lineFigure : public figure {
public:
	lineFigure(int x, int y, int h, int w) : figure(x, y, h, w) {
		XMFLOAT4 color = { 1.0f,0.0f,0.0f,1.0f };
		for (int i = 0; i < 4; i++) {
			
				blocks[i] = new block(color, { x,y + i });
			
		}
		Y++;
	}
	bool moveDown() {
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.y += 1;
			block* c = blocks[i]->Collision(p.x, p.y, height, width);
			if (c == blocks[i]) {
				return false;
			}
			if (c != NULL) {
				bool p = false;
				for (int j = 0; j < 4; j++) {
					if (c == blocks[j]) {
						p = true;
						break;
					}
				}
				if (!p) {
					return false;
				}
			}
		}
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.y += 1;
			blocks[i]->setCoord(p);
		}
		Y++;
		return true;
	}
	void moveLeft() {
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.x--;
			block* c = blocks[i]->Collision(p.x, p.y, height, width);
			if (c == blocks[i]) {
				return;
			}
			if (c != NULL) {
				bool p = false;
				for (int j = 0; j < 4; j++) {
					if (c == blocks[j]) {
						p = true;
						break;
					}
				}
				if (!p) {
					return;
				}
			}
		}
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.x--;
			blocks[i]->setCoord(p);
		}
		X--;
	}
	void moveRight() {
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.x++;
			block* c = blocks[i]->Collision(p.x, p.y, height, width);
			if (c == blocks[i]) {
				return;
			}
			if (c != NULL) {
				bool p = false;
				for (int j = 0; j < 4; j++) {
					if (c == blocks[j]) {
						p = true;
						break;
					}
				}
				if (!p) {
					return;
				}
			}
		}
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.x++;
			blocks[i]->setCoord(p);
		}
		X++;
	} //движение вправо
	void rotate() {
		figForRotate newFigPos;
		newFigPos.X = X;
		newFigPos.Y = Y;
		for (int i = 0; i < 4; i++) {
			newFigPos.blocksPos[i].x = (blocks[i]->getPos().y-Y)+X;
			newFigPos.blocksPos[i].y = -(blocks[i]->getPos().x-X)+Y;
		}
		bool pr = canStay(newFigPos);

		if (!pr) { //рассматриваем ближайшие позиции от -2 до 2 если текущая не свободная для повернутой фигуры
			offsetFigureX(newFigPos, 1);
			pr = canStay(newFigPos);
			if (!pr) {
				offsetFigureX(newFigPos, -2);
				pr = canStay(newFigPos);
				if (!pr) {
					offsetFigureX(newFigPos, 3);
					pr = canStay(newFigPos);
					if (!pr) {
						offsetFigureX(newFigPos, -4);
						pr = canStay(newFigPos);
					}
				}
			}
		}

		if (pr) {
			for (int i = 0; i < 4; i++) {
				blocks[i]->setCoord(newFigPos.blocksPos[i]);
			}
		}
	} //вращение фигуры
protected:
	bool canStay(figForRotate f) { //проверям можем ли разместить повернутую фигуру
		block* cheker = new block;
		for (int i = 0; i < 4; i++) {
			block* c = cheker->Collision(f.blocksPos[i].x, f.blocksPos[i].y, height, width);
			if (c != NULL) {
				bool pr = false;
				for (int i = 0; i < 4; i++) {
					if (blocks[i] == c) {
						pr = true;
						break;
					}
				}
				if (!pr) {
					delete cheker;
					return false;
				}
			}
		}
		delete cheker;
		return true;
	}
	void offsetFigureX(figForRotate& f, int dx) {
		for (int i = 0; i < 4; i++) {
			f.blocksPos[i].x += dx;
		}
	}
};


class GFigure : public figure {
public:
	GFigure(int x, int y, int h, int w) : figure(x, y, h, w) {
		XMFLOAT4 color = { 1.0f,0.0f,0.0f,1.0f };
		for (int i = 1; i < 4; i++) {

			blocks[i] = new block(color, { x,y + i-1 });

		}
		blocks[0] = new block(color, { x + 1,y });
		Y++;
	}
	bool moveDown() {
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.y += 1;
			block* c = blocks[i]->Collision(p.x, p.y, height, width);
			if (c == blocks[i]) {
				return false;
			}
			if (c != NULL) {
				bool p = false;
				for (int j = 0; j < 4; j++) {
					if (c == blocks[j]) {
						p = true;
						break;
					}
				}
				if (!p) {
					return false;
				}
			}
		}
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.y += 1;
			blocks[i]->setCoord(p);
		}
		Y++;
		return true;
	}
	void moveLeft() {
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.x--;
			block* c = blocks[i]->Collision(p.x, p.y, height, width);
			if (c == blocks[i]) {
				return;
			}
			if (c != NULL) {
				bool p = false;
				for (int j = 0; j < 4; j++) {
					if (c == blocks[j]) {
						p = true;
						break;
					}
				}
				if (!p) {
					return;
				}
			}
		}
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.x--;
			blocks[i]->setCoord(p);
		}
		X--;
	}
	void moveRight() {
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.x++;
			block* c = blocks[i]->Collision(p.x, p.y, height, width);
			if (c == blocks[i]) {
				return;
			}
			if (c != NULL) {
				bool p = false;
				for (int j = 0; j < 4; j++) {
					if (c == blocks[j]) {
						p = true;
						break;
					}
				}
				if (!p) {
					return;
				}
			}
		}
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.x++;
			blocks[i]->setCoord(p);
		}
		X++;
	} //движение вправо
	void rotate() {
		figForRotate newFigPos;
		newFigPos.X = X;
		newFigPos.Y = Y;
		for (int i = 0; i < 4; i++) {
			newFigPos.blocksPos[i].x = (blocks[i]->getPos().y - Y) + X;
			newFigPos.blocksPos[i].y = -(blocks[i]->getPos().x - X) + Y;
		}
		bool pr = canStay(newFigPos);

		if (!pr) { //рассматриваем ближайшие позиции от -2 до 2 если текущая не свободная для повернутой фигуры
			offsetFigureX(newFigPos, 1);
			pr = canStay(newFigPos);
			if (!pr) {
				offsetFigureX(newFigPos, -2);
				pr = canStay(newFigPos);
				if (!pr) {
					offsetFigureX(newFigPos, 3);
					pr = canStay(newFigPos);
					if (!pr) {
						offsetFigureX(newFigPos, -4);
						pr = canStay(newFigPos);
					}
				}
			}
		}

		if (pr) {
			for (int i = 0; i < 4; i++) {
				blocks[i]->setCoord(newFigPos.blocksPos[i]);
			}
		}
	} //вращение фигуры
protected:
	bool canStay(figForRotate f) { //проверям можем ли разместить повернутую фигуру
		block* cheker = new block;
		for (int i = 0; i < 4; i++) {
			block* c = cheker->Collision(f.blocksPos[i].x, f.blocksPos[i].y, height, width);
			if (c != NULL) {
				bool pr = false;
				for (int i = 0; i < 4; i++) {
					if (blocks[i] == c) {
						pr = true;
						break;
					}
				}
				if (!pr) {
					delete cheker;
					return false;
				}
			}
		}
		delete cheker;
		return true;
	}
	void offsetFigureX(figForRotate& f, int dx) {
		for (int i = 0; i < 4; i++) {
			f.blocksPos[i].x += dx;
		}
	}
};


class TFigure : public figure {
public:
	TFigure(int x, int y, int h, int w) : figure(x, y, h, w) {
		XMFLOAT4 color = { 1.0f,0.0f,0.0f,1.0f };
		for (int i = 1; i < 4; i++) {

			blocks[i] = new block(color, { x,y + i - 1 });

		}
		blocks[0] = new block(color, { x + 1,y+1 });
		Y++;
	}
	bool moveDown() {
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.y += 1;
			block* c = blocks[i]->Collision(p.x, p.y, height, width);
			if (c == blocks[i]) {
				return false;
			}
			if (c != NULL) {
				bool p = false;
				for (int j = 0; j < 4; j++) {
					if (c == blocks[j]) {
						p = true;
						break;
					}
				}
				if (!p) {
					return false;
				}
			}
		}
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.y += 1;
			blocks[i]->setCoord(p);
		}
		Y++;
		return true;
	}
	void moveLeft() {
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.x--;
			block* c = blocks[i]->Collision(p.x, p.y, height, width);
			if (c == blocks[i]) {
				return;
			}
			if (c != NULL) {
				bool p = false;
				for (int j = 0; j < 4; j++) {
					if (c == blocks[j]) {
						p = true;
						break;
					}
				}
				if (!p) {
					return;
				}
			}
		}
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.x--;
			blocks[i]->setCoord(p);
		}
		X--;
	}
	void moveRight() {
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.x++;
			block* c = blocks[i]->Collision(p.x, p.y, height, width);
			if (c == blocks[i]) {
				return;
			}
			if (c != NULL) {
				bool p = false;
				for (int j = 0; j < 4; j++) {
					if (c == blocks[j]) {
						p = true;
						break;
					}
				}
				if (!p) {
					return;
				}
			}
		}
		for (int i = 0; i < 4; i++) {
			Coord p = blocks[i]->getPos();
			p.x++;
			blocks[i]->setCoord(p);
		}
		X++;
	} //движение вправо
	void rotate() {
		figForRotate newFigPos;
		newFigPos.X = X;
		newFigPos.Y = Y;
		for (int i = 0; i < 4; i++) {
			newFigPos.blocksPos[i].x = (blocks[i]->getPos().y - Y) + X;
			newFigPos.blocksPos[i].y = -(blocks[i]->getPos().x - X) + Y;
		}
		bool pr = canStay(newFigPos);

		if (!pr) { //рассматриваем ближайшие позиции от -2 до 2 если текущая не свободная для повернутой фигуры
			offsetFigureX(newFigPos, 1);
			pr = canStay(newFigPos);
			if (!pr) {
				offsetFigureX(newFigPos, -2);
				pr = canStay(newFigPos);
				if (!pr) {
					offsetFigureX(newFigPos, 3);
					pr = canStay(newFigPos);
					if (!pr) {
						offsetFigureX(newFigPos, -4);
						pr = canStay(newFigPos);
					}
				}
			}
		}

		if (pr) {
			for (int i = 0; i < 4; i++) {
				blocks[i]->setCoord(newFigPos.blocksPos[i]);
			}
		}
	} //вращение фигуры
protected:
	bool canStay(figForRotate f) { //проверям можем ли разместить повернутую фигуру
		block* cheker = new block;
		for (int i = 0; i < 4; i++) {
			block* c = cheker->Collision(f.blocksPos[i].x, f.blocksPos[i].y, height, width);
			if (c != NULL) {
				bool pr = false;
				for (int i = 0; i < 4; i++) {
					if (blocks[i] == c) {
						pr = true;
						break;
					}
				}
				if (!pr) {
					delete cheker;
					return false;
				}
			}
		}
		delete cheker;
		return true;
	}
	void offsetFigureX(figForRotate& f, int dx) {
		for (int i = 0; i < 4; i++) {
			f.blocksPos[i].x += dx;
		}
	}
};



class blockDeleter{ //класс следящий за тем, когда надо удалить блоки, которые должны исчезнуть или удаляющий все блоки на поле
public:
	blockDeleter(float w, float h) {
		Height = h;
		Width = w;
	};
	~blockDeleter() {};
	void offsetBlocksDown(float Y) {
		block* cheker = new block;
		for (int i = Y; i >=0; i--) {
			for (int j = 0; j < Width; j++) {
				block* c = cheker->Collision(j, i, Height, Width);
				if (c != NULL) {
					Coord p = c->getPos();
					p.y++;
					c->setCoord(p);
				}
			}
		}
		delete cheker;
	}

	void checkOnDeletion() {
		block* cheker = new block;
		for (int i = Height; i >=0 ; i--) {
			bool pr = true;
			for (int j = 0; j < Width; j++) {
				block* c = cheker->Collision(j, i, Height, Width);
				if (c == NULL) {
					pr = false;
				}
			}
			if (pr) {
				for (int j = 0; j < Width; j++) {
					block* c = cheker->Collision(j, i, Height, Width);
					delete c;
				}
				offsetBlocksDown(i - 1);
				i++;
			}
		}
		delete cheker;
	}

	void deleteAllBlocks() {
		block* cheker = new block;
		for (int i = Height; i >= -5; i--) {
	
			for (int j = 0; j < Width; j++) {
				block* c = cheker->Collision(j, i, Height, Width);
				while (c != NULL && c!=cheker) {
					delete c;
					c = cheker->Collision(j, i, Height, Width);
				}
			}
			
		}
		delete cheker;
	}

protected:
	float Height;
	float Width;
};