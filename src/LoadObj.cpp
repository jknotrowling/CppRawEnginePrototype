

#include "Classes.h"

extern sur::Map_Analyses _Amap;
extern std::vector<int> identitys;	
extern std::vector<void*> ptrs;


void sur::LoadObj::Load()
{
	std::string Data = "";
	std::string Nums = "0123456789";
	std::string ColorRef = "";
	std::ifstream file(Path);
	if (!file) {
		std::string errstr = "Object path not found; Path given: ";
		errstr += Path;
		errstr += "; Object name -> ";
		errstr += typeid(this).name();	
		Error(errstr.c_str());
	}

	std::ostringstream ss;
	ss << file.rdbuf();
	Data = ss.str();
	file.close();

	int count;
	for (int i = 0; i < Data.size(); i++) {
		if (Data[i] == '-') {
			x++;
		}
		if (Data[i] == 'E') {
			y++;	
			MaxX->push_back(x);
			x = 0;
		}
		if (Data[i] == '/') {
			for (int j = 1;; j++) {
				count = 0;
				for (int a = 0; a < Nums.size(); a++) {
					if (Data[i + j] == Nums[a]) {
						ColorRef += Data[i + j];
						count++;
					}
					else if (a == Nums.size() - 1 && count == 0) {
						goto Leave;
					}
				}
			}
		Leave:
			XCoords->push_back(x + position.x);
			YCoords->push_back(y + position.y);
			Colors->push_back(std::stoi(ColorRef));
			x++;
			ColorRef = "";
		}
	}
	size.x = *std::max_element(MaxX->begin(), MaxX->end());
	size.y = y;
	delete MaxX;
}

sur::LoadObj::LoadObj(const char* Path, sur::Vec2 position, std::string name, int id) 
	: Path(Path), Master(name, id, position)
{
	identitys.push_back(id);
	ptrs.push_back(this);
	Load();
}

void sur::LoadObj::MoveInject(int index, int CurMove)
{
	switch (index)
	{
	case 1:
		position.y -= CurMove;
		for (int i = 0; i < YCoords->size(); i++)
			YCoords->at(i) -= CurMove;
		break;
	case 2:
		position.x += CurMove;
		for (int i = 0; i < XCoords->size(); i++)
			XCoords->at(i) += CurMove;
		break;
	case 3:
		position.y += CurMove;
		for (int i = 0; i < YCoords->size(); i++)
			YCoords->at(i) += CurMove;
		break;
	case 4:
		position.x -= CurMove;
		for (int i = 0; i < XCoords->size(); i++)
			XCoords->at(i) -= CurMove;
		break;
	}
}

void sur::LoadObj::Bind(bool Collider, ColliderType collidertype)
{
	if (collidertype == ColliderType::Static) {
		for (int i = 0; i < YCoords->size(); i++) {
			if (XCoords->at(i) >= 1 && YCoords->at(i) >= 1 && XCoords->at(i) <= _window_size.x - 1 && YCoords->at(i) <= _window_size.y - 1)
				if(Debug)
					_Amap.Render(XCoords->at(i), YCoords->at(i), Color(255,255,255));
				else
					_Amap.Render(XCoords->at(i), YCoords->at(i), Colors->at(i).ToCOLORREF());
				if (XCoords->at(i) > 1 && YCoords->at(i) > 1 && XCoords->at(i) < _window_size.x - 1 && YCoords->at(i) < _window_size.y - 1)			
					_Amap.Collider(XCoords->at(i) - 1, YCoords->at(i) - 1, id);
		}
		return;
	}
	if (collidertype == ColliderType::Outline) {	// Outlined Collider -> Good for Objects form outside.
		for (int i = 0; i < YCoords->size(); i++)
			if (XCoords->at(i) >= 1 && YCoords->at(i) >= 1 && XCoords->at(i) <= _window_size.x - 1 && YCoords->at(i) <= _window_size.y - 1)
				_Amap.Render(XCoords->at(i), YCoords->at(i), Colors->at(i).ToCOLORREF());
		for (int i = position.x; i < size.x + position.x; i++) {
			if (i > 1 && position.y > 1 && i <= _window_size.x - 1 && position.y <= _window_size.y - 1) {
				if(Debug)
					_Amap.Render(i, position.y, Color(255,255,255));
				_Amap.Collider(i - 1, position.y - 1, id);
			}
		}
		for (int i = position.y; i < size.y + position.y; i++) {
			if (i > 1 && position.x > 1 && i <= _window_size.x - 1 && position.x <= _window_size.y - 1) {
				if(Debug)
					_Amap.Render(position.x, i, Color(255, 255, 255));
				_Amap.Collider(position.x - 1, i - 1, id);
			}
		}
		for (int i = position.x; i < size.x + position.x; i++) {
			if (i > 1 && size.y + position.y > 1 && i <= _window_size.x - 1 && size.y + position.y <= _window_size.y - 1){
				if(Debug)
					_Amap.Render(i, size.y + position.y, Color(255, 255, 255));
				_Amap.Collider(i - 1, size.y - 1 + position.y, id);
			}
		}
		for (int i = position.y; i < size.y + position.y; i++) {
			if (i > 1 && size.x + position.x > 1 && i <= _window_size.x - 1 && size.x + position.x <= _window_size.y - 1) {
				if(Debug)
					_Amap.Render(size.x + position.x, i, Color(255, 255, 255));
				_Amap.Collider(size.x - 1 + position.x, i - 1, id);
			}
		}
		return;
	}
	if (collidertype == ColliderType::Filled) {
		for (int i = 0; i < YCoords->size(); i++) 
			if (XCoords->at(i) >= 1 && YCoords->at(i) >= 1 && XCoords->at(i) <= _window_size.x - 1 && YCoords->at(i) <= _window_size.y - 1)
				_Amap.Render(XCoords->at(i), YCoords->at(i), Colors->at(i).ToCOLORREF());
		for (int a = position.y; a < position.y + size.y; a++) {
			for (int b = position.x; b < position.x + size.x; b++) {
				if (a > 1 && b > 1 && a < _window_size.x - 1 && b < _window_size.y - 1) {
					if (Debug)
						_Amap.Render(b, a, Color(255, 255, 255));
					_Amap.Collider(b - 1, a - 1, id);
				}
			}
		}
		return;
	}
}

//void sur::LoadObj::Move(sur::Vec2 direction)
//{
//	for (int i = 0; i < YCoords->size(); i++) {
//		YCoords->at(i) -= direction.y;
//		XCoords->at(i) += direction.x;
//	}
//	position.y -= direction.y;
//	position.x += direction.x;
//}
