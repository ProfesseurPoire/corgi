#include <corgi/ui/layouts/GridLayout.h>

#include <cmath>

corgi::ui::GridLayout::GridLayout()
{
}

void corgi::ui::GridLayout::update(float elapsed_time)
{
	/*unsigned index=0;
	
	float offset_x		= 0.0f;
	float offset_y		= 0.0f;
	float max_height	= 0.0f;

	if(!fit_to_content)
	{
		float widget_width  =  (width_ - (static_cast<float>(columns)+1.0f)*margin)/static_cast<float>(columns);
		float widget_height =  (height_- (static_cast<float>(rows)+1.0f)*margin)/static_cast<float>(rows);
		
		for(auto& child : children_)
		{
			auto* widget  =  dynamic_cast<Widget*>(child.get());
			widget->width(widget_width);
			widget->height(widget_height);
		}
	}
	
	for(auto& child : children_)
	{
		auto* widget = dynamic_cast<Widget*>(child.get());
		
		const unsigned current_column	= index % columns;
		const unsigned current_row		=  static_cast<unsigned>(floor(static_cast<float>(index) / static_cast<float>(columns)));
	
		if(current_column==0)
		{
			offset_x =0;
			offset_y+= max_height;
			max_height=0;
		}

		offset_x += margin;

		if(widget->height()> max_height)
			max_height=widget->height() + margin;
			
		widget->position(offset_x,offset_y+margin);
		offset_x+= get_column_max_widget(current_column);
		index++;
	}*/
}

float corgi::ui::GridLayout::get_column_max_widget(unsigned column_id)
{
	float max_width=0.0f;
	
	/*for(unsigned i=0u; i< rows; i++)
	{
		unsigned cellid =  column_id+ i*columns;

		auto* widget = dynamic_cast<Widget*>( children_.at(cellid).get());

		if(widget->width()>max_width)
		{
			max_width=widget->width();
		}
	}*/
	return max_width;
}
