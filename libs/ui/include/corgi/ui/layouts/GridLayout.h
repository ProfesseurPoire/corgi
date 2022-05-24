#pragma once

#include <corgi/ui/Widget.h>

namespace corgi::ui
{
	class GridLayout : public Widget
	{
	public:

		GridLayout();

		void update(float elapsed_time) override;

		unsigned	rows;
		unsigned	columns;
		float		margin;
		bool		fit_to_content{false};

	private:

		float get_column_max_widget(unsigned column_id);
	};
}