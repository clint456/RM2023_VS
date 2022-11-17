#include "../inc/Armor.h"
armorBox::armorBox(const lightBar& l_light, const lightBar& r_light)  {
	this->center = (l_light.center + r_light.center) / 2;
	this->l_light = l_light;
	this->r_light = r_light;
    
}

armorBox::armorBox() {


}
