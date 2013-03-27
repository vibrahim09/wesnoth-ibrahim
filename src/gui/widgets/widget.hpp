/*
   Copyright (C) 2007 - 2013 by Mark de Wever <koraq@xs4all.nl>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef GUI_WIDGETS_WIDGET_HPP_INCLUDED
#define GUI_WIDGETS_WIDGET_HPP_INCLUDED

#include "gui/auxiliary/event/dispatcher.hpp"
#include "gui/lib/types/point.hpp"
#include "gui/widgets/event_executor.hpp"

#include "sdl_utils.hpp"

#include <boost/noncopyable.hpp>

#include <string>

namespace gui2 {

struct tbuilder_widget;
class tdialog;
class twindow;

namespace iterator {
	class twalker_;
} // namespace iterator

/**
 * Base class for all widgets.
 *
 * From this abstract all other widgets should derive. It contains the minimal
 * info needed for a real widget and some pure abstract functions which need to
 * be implemented by classes deriving from this class.
 */
class twidget
	: private boost::noncopyable
	, public tevent_executor
	, public event::tdispatcher
{
	friend class tdebug_layout_graph;
	friend class twindow; // needed for modifying the layout_size.


	/***** ***** ***** ***** ***** Types. ***** ***** ***** ***** *****/

public:

	/** Visibility settings done by the user. */
	class tvisible
		: private boost::noncopyable
	{
		friend class tno_such_friend_exists_but_it_makes_the_compiler_happy;
		tvisible();
	public:

		/** @todo C++11 use a scoped enum. */
		enum scoped_enum {
			/**
			 * The user sets the widget visible, that means:
			 * * The widget is visible.
			 * * @ref find_at always 'sees' the widget (the active flag is
			 *   tested later).
			 * * The widget (if active) handles events (and sends events to
			 *   its children).
			 * * The widget is drawn (and sends the call to
			 *   @ref populate_dirty_list to children).
			 */
			visible,

			/**
			 * The user sets the widget hidden, that means:
			 * * The widget is invisible but keeps its size.
			 * * @ref find_at 'sees' the widget if active is @c false.
			 * * The widget doesn't handle events (and doesn't send events to
			 *   its children).
			 * * The widget doesn't add itself @ref twindow::dirty_list_ when
			 *   @ref populate_dirty_list is called (nor does it send the
			 *   request to its children).
			 */
			hidden,

			/**
			 * The user set the widget invisible, that means:
			 * * The widget is invisible and its grid cell has size 0,0.
			 * * @ref find_at never 'sees' the widget.
			 * * The widget doesn't handle events (and doesn't send events to
			 *   its children).
			 * * The widget doesn't add itself @ref twindow::dirty_list_ when
			 *   @ref populate_dirty_list is called (nor does it send the
			 *   request to its children).
			 */
			invisible
		};
	};

	/**
	 * Visibility set by the engine.
	 *
	 * This state only will be used if @ref visible_ is @ref tvisible::visible
	 * depending on this state the widget might not be visible after all.
	 */
	class tredraw_action
		: private boost::noncopyable
	{
		friend class tno_such_friend_exists_but_it_makes_the_compiler_happy;
		tredraw_action();
	public:

		/** @todo C++11 use a scoped enum. */
		enum scoped_enum {
			/**
			 * The widget is fully visible.
			 *
			 * The widget should be drawn if @ref dirty_ is @c true. The entire
			 * widget's rectangle should be redrawn.
			 */
			full,

			/**
			 * The widget is partly visible.
			 *
			 * The should be drawn if @ref dirty_ is @c true. The rectangle to
			 * redraw in determined by @ref clipping_rectangle_
			 */
			partly,

			/**
			 * The widget is not visible.
			 *
			 * The widget should not be drawn if @ref dirty_ is @c true.
			 */
			none
		};
	};


	/***** ***** ***** Constructor and destructor. ***** ***** *****/

public:

	/** @deprecated use the second overload. */
	twidget();

	/**
	 * Constructor.
	 *
	 * @param builder             The builder object with the settings for the
	 *                            object.
	 */
	explicit twidget(const tbuilder_widget& builder);

	virtual ~twidget() OVERRIDE;


	/***** ***** ***** ***** ID functions. ***** ***** ***** *****/

public:

	/*** *** *** *** *** *** Setters and getters. *** *** *** *** *** ***/

	void set_id(const std::string& id);
	const std::string& id() const;

	/*** *** *** *** *** *** *** *** Members. *** *** *** *** *** *** *** ***/

private:

	/**
	 * The id is the unique name of the widget in a certain context.
	 *
	 * This is needed for certain widgets so the engine knows which widget is
	 * which. E.g. it knows which button is pressed and thus which engine action
	 * is connected to the button. This doesn't mean that the id is unique in a
	 * window, e.g. a listbox can have the same id for every row.
	 */
	std::string id_;


	/***** ***** ***** ***** Parent functions ***** ***** ***** *****/

public:

	/**
	 * Get the parent window.
	 *
	 * @returns                   Pointer to parent window.
	 * @retval NULL               No parent window found.
	 */
	twindow* get_window();

	/** The constant version of @ref get_window. */
	const twindow* get_window() const;

	/**
	 * Returns the top-level dialogue.
	 *
	 * A window is most of the time created by a dialogue, this function returns
	 * that dialogue.
	 *
	 * @deprecated The function was used to install callbacks to member
	 * functions of the dialogue. Once all widgets are converted to signals this
	 * function will be removed.
	 *
	 * @returns                   The top-level dialogue.
	 * @retval NULL               No top-level window or the top-level window is
	 *                            not owned by a dialogue.
	 */
	tdialog* dialog();

	/*** *** *** *** *** *** Setters and getters. *** *** *** *** *** ***/

	void set_parent(twidget* parent);
	twidget* parent();

	/*** *** *** *** *** *** *** *** Members. *** *** *** *** *** *** *** ***/

private:

	/**
	 * The parent widget.
	 *
	 * If the widget has a parent it contains a pointer to the parent, else it
	 * is set to @c NULL.
	 */
	twidget* parent_;


	/***** ***** ***** ***** Size and layout functions. ***** ***** ***** *****/

public:

	/**
	 * How the layout engine works.
	 *
	 * Every widget has a member @ref layout_size_ which holds the best size in
	 * the current layout phase. When the windows starts the layout phase it
	 * calls @ref layout_initialise which resets this value.
	 *
	 * Every widget has two function to get the best size. @ref get_best_size
	 * tests whether layout_size_ is set and if so returns that value otherwise
	 * it calls @ref calculate_best_size so the size can be updated.
	 *
	 * During the layout phase some functions can modify layout_size_ so the
	 * next call to @ref get_best_size returns the currently best size. This
	 * means that after the layout phase @ref get_best_size still returns this
	 * value.
	 */

	/**
	 * Initialises the layout phase.
	 *
	 * Clears the initial best size for the widgets.
	 *
	 * @see @ref layout_algorithm for more information.
	 *
	 * @param full_initialisation For widgets with scrollbars it hides them
	 *                            unless the mode is
	 *                            @ref tscrollbar_mode::always_visible. For
	 *                            other widgets this flag is a @em NOP.
	 */
	virtual void layout_initialise(const bool full_initialisation);

	/**
	 * Tries to reduce the width of a widget.
	 *
	 * This function tries to do it 'friendly' and only use scrollbars or
	 * tries to wrap the widget.
	 *
	 * @see @ref layout_algorithm for more information.
	 *
	 * @param maximum_width       The wanted maximum width.
	 */
	virtual void request_reduce_width(const unsigned maximum_width) = 0;

	/**
	 * Tries to reduce the width of a widget.
	 *
	 * This function does it more aggressively and should only be used when
	 * using scrollbars and wrapping failed.
	 *
	 * @todo Make pure virtual.
	 *
	 * @see @ref layout_algorithm for more information.
	 *
	 * @param maximum_width       The wanted maximum width.
	 */
	virtual void demand_reduce_width(const unsigned maximum_width);

	/**
	 * Tries to reduce the height of a widget.
	 *
	 * This function tries to do it 'friendly' and only use scrollbars.
	 *
	 * @todo Make pure virtual.
	 *
	 * @see @ref layout_algorithm for more information.
	 *
	 * @param maximum_height      The wanted maximum height.
	 */
	virtual void request_reduce_height(const unsigned maximum_height);

	/**
	 * Tries to reduce the height of a widget.
	 *
	 * This function does it more aggressively and should only be used when
	 * using scrollbars failed.
	 *
	 * @todo Make pure virtual.
	 *
	 * @see @ref layout_algorithm for more information.
	 *
	 * @param maximum_height      The wanted maximum height.
	 */
	virtual void demand_reduce_height(const unsigned maximum_height);

	/**
	 * Gets the best size for the widget.
	 *
	 * During the layout phase a best size will be determined, several stages
	 * might change the best size. This function will return the currently best
	 * size as determined during the layout phase.
	 *
	 * @returns                      The best size for the widget.
	 * @retval 0,0                   The best size is 0,0.
	 */
	tpoint get_best_size() const;

private:

	/**
	 * Calculates the best size.
	 *
	 * This function calculates the best size and ignores the current values in
	 * the layout phase. Note containers can call the @ref get_best_size() of
	 * their children since it is meant to update itself.
	 *
	 * @returns                      The best size for the widget.
	 * @retval 0,0                   The best size is 0,0.
	 */
	virtual tpoint calculate_best_size() const = 0;

public:

	/**
	 * Can the widget wrap.
	 *
	 * When a widget can wrap it can reduce its width by increasing its
	 * height. When a layout is too wide it should first try to wrap and if
	 * that fails it should check the vertical scrollbar status. After wrapping
	 * the height might (probably will) change so the layout engine needs to
	 * recalculate the height after wrapping.
	 */
	virtual bool can_wrap() const;

	/**
	 * Sets the origin of the widget.
	 *
	 * This function can be used to move the widget without dirtying it. The
	 * location is an absolute position, if a relative more is required use
	 * @ref move.
	 *
	 *
	 * @param origin              The new origin.
	 */
	virtual void set_origin(const tpoint& origin);

	/**
	 * Sets the size of the widget.
	 *
	 * This version is meant to resize a widget, since the origin isn't
	 * modified. This can be used if a widget needs to change its size and the
	 * layout will be fixed later.
	 *
	 * @param size                The size of the widget.
	 */
	virtual void set_size(const tpoint& size);

	/**
	 * Places the widget.
	 *
	 * This function is normally called by a layout function to do the
	 * placement of a widget.
	 *
	 * @param origin              The position of top left of the widget.
	 * @param size                The size of the widget.
	 */
	virtual void place(const tpoint& origin, const tpoint& size);

	/**
	 * Moves a widget.
	 *
	 * This function can be used to move the widget without dirtying it.
	 *
	 * @todo Implement the function to all derived classes.
	 *
	 * @param x_offset            The amount of pixels to move the widget in
	 *                            the x-direction.
	 * @param y_offset            The amount of pixels to move the widget in
	 *                            the y-direction.
	 */
	virtual void move(const int x_offset, const int y_offset);

	/**
	 * Allows a widget to update its children.
	 *
	 * Before the window is populating the dirty list the widgets can update
	 * their content, which allows delayed initialization. This delayed
	 * initialization is only allowed if the widget resizes itself, not when
	 * being placed.
	 */
	virtual void layout_children();

	/**
	 * Returns the screen origin of the widget.
	 *
	 * @returns                   The origin of the widget.
	 */
	tpoint get_origin() const;

	/**
	 * Returns the size of the widget.
	 *
	 * @returns                   The size of the widget.
	 */
	tpoint get_size() const;

	/**
	 * Gets the bounding rectangle of the widget on the screen.
	 *
	 * @returns                   The bounding rectangle of the widget.
	 */
	SDL_Rect get_rectangle() const;

	/*** *** *** *** *** *** Setters and getters. *** *** *** *** *** ***/

	int get_x() const;

	int get_y() const;

	unsigned get_width() const;

	unsigned get_height() const;

protected:

	void set_layout_size(const tpoint& size);
	const tpoint& layout_size() const;

public:

	void set_linked_group(const std::string& linked_group);

	/*** *** *** *** *** *** *** *** Members. *** *** *** *** *** *** *** ***/

private:

	/** The x-coordinate of the widget on the screen. */
	int x_;

	/** The y-coordinate of the widget on the screen. */
	int y_;

	/** The width of the widget. */
	unsigned width_;

	/** The height of the widget. */
	unsigned height_;

	/**
	 * The best size for the widget.
	 *
	 * When 0,0 the real best size is returned, but in the layout phase a
	 * wrapping or a scrollbar might change the best size for that widget.
	 * This variable holds that best value.
	 */
	tpoint layout_size_;

#ifdef DEBUG_WINDOW_LAYOUT_GRAPHS

	/**
	 * Debug helper to store last value of get_best_size().
	 *
	 * We're mutable so calls can stay const and this is disabled in
	 * production code.
	 */
	mutable tpoint last_best_size_;

#endif

	/**
	 * The linked group the widget belongs to.
	 *
	 * @todo For now the linked group is initialised when the layout of the
	 * widget is initialised. The best time to set it would be upon adding the
	 * widget in the window. Need to look whether it is possible in a clean way.
	 * Maybe a signal just prior to showing a window where the widget can do
	 * some of it's on things, would also be nice for widgets that need a
	 * finaliser function.
	 */
	std::string linked_group_;


	/***** ***** ***** ***** Drawing functions. ***** ***** ***** *****/

public:

	/**
	 * Calculates the blitting rectangle of the widget.
	 *
	 * The blitting rectangle is the entire widget rectangle, but offsetted for
	 * drawing position.
	 *
	 * @param x_offset            The offset in the x-direction when drawn.
	 * @param y_offset            The offset in the y-direction when drawn.
	 *
	 * @returns                   The drawing rectangle.
	 */
	SDL_Rect calculate_blitting_rectangle(
			  const int x_offset
			, const int y_offset);

	/**
	 * Calculates the clipping rectangle of the widget.
	 *
	 * The clipping rectangle is used then the @ref redraw_action_ is
	 * @ref tredraw_action::partly. Since the drawing can be offsetted it also
	 * needs offset paramters.
	 *
	 * @param x_offset            The offset in the x-direction when drawn.
	 * @param y_offset            The offset in the y-direction when drawn.
	 *
	 * @returns                   The clipping rectangle.
	 */
	SDL_Rect calculate_clipping_rectangle(
			  const int x_offset
			, const int y_offset);

	/**
	 * Draws the background of a widget.
	 *
	 * Derived should override @ref impl_draw_background instead of changing
	 * this function.
	 *
	 * @param frame_buffer        The surface to draw upon.
	 * @param x_offset            The offset in the x-direction in the
	 *                            @p frame_buffer to draw.
	 * @param y_offset            The offset in the y-direction in the
	 *                            @p frame_buffer to draw.
	 */
	void draw_background(surface& frame_buffer, int x_offset, int y_offset);
	void draw_background(surface& frame_buffer);

	/**
	 * Draws the children of a widget.
	 *
	 * Containers should draw their children when they get this request.
	 *
	 * Derived should override @ref impl_draw_children instead of changing
	 * this function.
	 *
	 * @param frame_buffer        The surface to draw upon.
	 * @param x_offset            The offset in the x-direction in the
	 *                            @p frame_buffer to draw.
	 * @param y_offset            The offset in the y-direction in the
	 *                            @p frame_buffer to draw.
	 */
	void draw_children(surface& frame_buffer, int x_offset, int y_offset);
	void draw_children(surface& frame_buffer);

	/**
	 * Draws the foreground of the widget.
	 *
	 * Some widgets e.g. panel and window have a back and foreground layer this
	 * function requests the drawing of the foreground.
	 *
	 * Derived should override @ref impl_draw_foreground instead of changing
	 * this function.
	 *
	 * @param frame_buffer        The surface to draw upon.
	 * @param x_offset            The offset in the x-direction in the
	 *                            @p frame_buffer to draw.
	 * @param y_offset            The offset in the y-direction in the
	 *                            @p frame_buffer to draw.
	 */
	void draw_foreground(surface& frame_buffer, int x_offset, int y_offset);
	void draw_foreground(surface& frame_buffer);

private:

	/** See @ref draw_background. */
	virtual void impl_draw_background(surface& /*frame_buffer*/) {}
	virtual void impl_draw_background(
			  surface& /*frame_buffer*/
			, int /*x_offset*/
			, int /*y_offset*/)
	{
	}

	/** See @ref draw_children. */
	virtual void impl_draw_children(surface& /*frame_buffer*/) {}
	virtual void impl_draw_children(
			surface& /*frame_buffer*/
			, int /*x_offset*/
			, int /*y_offset*/)
	{
	}

	/** See @ref draw_foreground. */
	virtual void impl_draw_foreground(surface& /*frame_buffer*/) {}
	virtual void impl_draw_foreground(
			  surface& /*frame_buffer*/
			, int /*x_offset*/
			, int /*y_offset*/)
	{
	}

public:

	/**
	 * Adds a widget to the dirty list if it is dirty.
	 *
	 * See @ref twindow::dirty_list_ for more information regarding the dirty
	 * list.
	 *
	 * If the widget is not dirty and has children it should add itself to the
	 * call_stack and call child_populate_dirty_list with the new call_stack.
	 *
	 * @param caller              The parent window, if dirty it should
	 *                            register itself to this window.
	 * @param call_stack          The call-stack of widgets traversed to reach
	 *                            this function.
	 */
	void populate_dirty_list(
			  twindow& caller
			, std::vector<twidget*>& call_stack);

private:

	/**
	 * Tries to add all children of a container to the dirty list.
	 *
	 * @note The function is private since everybody should call
	 * @ref populate_dirty_list instead.
	 *
	 * @param caller              The parent window, if dirty it should
	 *                            register itself to this window.
	 * @param call_stack          The call-stack of widgets traversed to reach
	 *                            this function.
	 */
	virtual void child_populate_dirty_list(
			  twindow& caller
			, const std::vector<twidget*>& call_stack);

public:

	/**
	 * Gets the dirty rectangle of the widget.
	 *
	 * Depending on the @ref redraw_action_ it returns the rectangle this
	 * widget dirties while redrawing.
	 *
	 * @returns                   The dirty rectangle.
	 */
	SDL_Rect get_dirty_rectangle() const;

	/**
	 * Sets the visible rectangle for a widget.
	 *
	 * This function sets the @ref redraw_action_ and the
	 * @ref clipping_rectangle_.
	 *
	 * @param rectangle           The visible rectangle in screen coordinates.
	 */
	virtual void set_visible_rectangle(const SDL_Rect& rectangle);

	/*** *** *** *** *** *** Setters and getters. *** *** *** *** *** ***/

	void set_dirty(const bool dirty);
	bool get_dirty() const;

	void set_visible(const tvisible::scoped_enum visible);
	tvisible::scoped_enum get_visible() const;

	tredraw_action::scoped_enum get_drawing_action() const;

#ifndef LOW_MEM

	void set_debug_border_mode(const unsigned debug_border_mode);

	void set_debug_border_colour(const unsigned debug_border_colour);

#endif

	/*** *** *** *** *** *** *** *** Members. *** *** *** *** *** *** *** ***/

private:

	/**
	 * Is the widget dirty?
	 *
	 * When a widget is dirty it needs to be redrawn at the next drawing cycle.
	 *
	 * The top-level window will use @ref populate_dirty_list and
	 * @ref child_populate_dirty_list to find al dirty widgets, so the widget
	 * doesn't need to inform its parent regarding it being marked dirty.
	 */
	bool dirty_;

	/** Field for the status of the visibility. */
	tvisible::scoped_enum visible_;

	/** Field for the action to do on a drawing request. */
	tredraw_action::scoped_enum redraw_action_;

	/** The clipping rectangle if a widget is partly visible. */
	SDL_Rect clipping_rectangle_;

#ifndef LOW_MEM

	/**
	 * Mode for drawing the debug border.
	 *
	 * The debug border is a helper border to determine where a widget is
	 * placed. It is only intended for debugging purposes.
	 *
	 * Possible values:
	 * - 0 no border
	 * - 1 single pixel border
	 * - 2 flood-filled rectangle
	 */
	unsigned debug_border_mode_;

	/** The colour for the debug border. */
	unsigned debug_border_colour_;

	void draw_debug_border(surface& frame_buffer);
	void draw_debug_border(surface& frame_buffer, int x_offset, int y_offset);

#else

	void draw_debug_border(surface&) {}
	void draw_debug_border(surface&, int, int) {}

#endif


	/***** ***** ***** ***** Query functions ***** ***** ***** *****/

public:

	/**
	 * Returns the widget at the wanted coordinates.
	 *
	 * @param coordinate          The coordinate which should be inside the
	 *                            widget.
	 * @param must_be_active      The widget should be active, not all widgets
	 *                            have an active flag, those who don't ignore
	 *                            flag.
	 *
	 * @returns                   The widget with the id.
	 * @retval NULL               No widget at the wanted coordinate found (or
	 *                            not active if must_be_active was set).
	 */
	virtual twidget* find_at(
			  const tpoint& coordinate
			, const bool must_be_active);

	/** The constant version of @ref find_at. */
	virtual const twidget* find_at(
			  const tpoint& coordinate
			, const bool must_be_active) const;

	/**
	 * Returns @em a widget with the wanted id.
	 *
	 * @note Since the id might not be unique inside a container there is no
	 * guarantee which widget is returned.
	 *
	 * @param id                  The id of the widget to find.
	 * @param must_be_active      The widget should be active, not all widgets
	 *                            have an active flag, those who don't ignore
	 *                            flag.
	 *
	 * @returns                   The widget with the id.
	 * @retval NULL               No widget with the id found (or not active if
	 *                            must_be_active was set).
	 */
	virtual twidget* find(
			  const std::string& id
			, const bool must_be_active);

	/** The constant version of @ref find. */
	virtual const twidget* find(
			  const std::string& id
			, const bool must_be_active) const;

	/**
	 * Does the widget contain the widget.
	 *
	 * Widgets can be containers which have more widgets inside them, this
	 * function will traverse in those child widgets and tries to find the
	 * wanted widget.
	 * 
	 * @param widget              Pointer to the widget to find.
	 *
	 * @returns                   Whether or not the @p widget was found.
	 */
	virtual bool has_widget(const twidget* widget) const;

private:

	/** See @ref event::tdispatcher::is_at. */
	virtual bool is_at(const tpoint& coordinate) const OVERRIDE;

	/**
	 * Is the coordinate inside our area.
	 *
	 * Helper for find_at so also looks at our visibility.
	 *
	 * @param coordinate          The coordinate which should be inside the
	 *                            widget.
	 * @param must_be_active      The widget should be active, not all widgets
	 *                            have an active flag, those who don't ignore
	 *                            flag.
	 *
	 * @returns                   Status.
	 */
	bool is_at(const tpoint& coordinate, const bool must_be_active) const;


	/***** ***** ***** ***** Miscellaneous ***** ***** ****** *****/

public:

	/** Does the widget disable easy close? */
	virtual bool disable_click_dismiss() const = 0;

	/** Creates a new walker object on the heap. */
	virtual iterator::twalker_* create_walker() = 0;
};

} // namespace gui2

#endif
