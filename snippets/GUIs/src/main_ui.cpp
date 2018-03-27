/**
 * MIT License
 *
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "main_ui.hpp"

#include <render_pipeline/rppanda/gui/direct_button.hpp>
#include <render_pipeline/rppanda/gui/direct_slider.hpp>
#include <render_pipeline/rppanda/gui/direct_check_box.hpp>
#include <render_pipeline/rppanda/gui/direct_entry.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/loader.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>

MainUI::MainUI(rpcore::RenderPipeline& pipeline): pipeline_(pipeline)
{
    ui_root_ = rpcore::Globals::base->get_aspect_2d().attach_new_node("main_ui_root");

    // create OnscreenText using constructor
    button_text_ = rppanda::OnscreenText(
        "Button Pressed: 0", rppanda::OnscreenText::Default::style, LVecBase2(1.0, 0.07f), 0,
        LVecBase2(0.05f), LColor(1, 1, 1, 1), {},
        LColor(0, 0, 0, 1), rppanda::OnscreenText::Default::shadow_offset, {},
        {}, {}, {}, false,
        {}, ui_root_);

    // create button
    auto button_options = std::make_shared<rppanda::DirectButton::Options>();
    button_options->pos = LVecBase3(1.0f, 0.0f, 0.0f);
    button_options->text ={ "Button" };
    button_options->scale = 0.05f;
    button_options->pad = LVecBase2(0.2f, 0.2f);
    button_options->command = std::bind(&MainUI::button_on_clicked, this);
    button_ = new rppanda::DirectButton(ui_root_, button_options);

    // create slider
    auto slider_options = std::make_shared<rppanda::DirectSlider::Options>();
    slider_options->pos = LVecBase3(1.0f, 0.0f, -0.07f);
    slider_options->scale = 0.4f;
    slider_options->value = pipeline_.get_daytime_mgr()->get_time();
    slider_options->command = std::bind(&MainUI::slider_on_changed, this);
    slider_ = new rppanda::DirectSlider(ui_root_, slider_options);

    // create OnscreenText using methods
    checkbox_text_ = rppanda::OnscreenText("CheckBox Status: Off");
    checkbox_text_.reparent_to(ui_root_);
    checkbox_text_.set_pos(LVecBase2(1.0f, -0.15f));
    checkbox_text_.set_scale(LVecBase2(0.04f));
    checkbox_text_.set_fg(LColor(1, 1, 1, 1));
    checkbox_text_.set_shadow(LColor(0, 0, 0, 1));
    checkbox_text_.set_align(TextProperties::A_left);

    // create checkbox
    auto checked_img = rpcore::RPLoader::load_texture("/$$rp/data/gui/checkbox_checked.png");
    auto unchecked_img = rpcore::RPLoader::load_texture("/$$rp/data/gui/checkbox_default.png");

    auto checkbox_options = std::make_shared<rppanda::DirectCheckBox::Options>();
    checkbox_options->pos = LVecBase3f(0.95f, 0.0f, -0.15f);
    checkbox_options->scale = LVecBase3f(0.03f);
    checkbox_options->checked_image = std::make_shared<rppanda::ImageInput>(checked_img);
    checkbox_options->unchecked_image = std::make_shared<rppanda::ImageInput>(unchecked_img);
    checkbox_options->image ={ std::make_shared<rppanda::ImageInput>(unchecked_img) };
    checkbox_options->checkbox_command = std::bind(&MainUI::checkbox_on_clicked, this, std::placeholders::_1);
    checkbox_ = new rppanda::DirectCheckBox(ui_root_, checkbox_options);

    // create OnscreenText using methods
    entry_text_ = rppanda::OnscreenText("Entry: ");
    entry_text_.reparent_to(ui_root_);
    entry_text_.set_pos(LVecBase2(0.8f, -0.24f));
    entry_text_.set_scale(LVecBase2(0.04f));
    entry_text_.set_fg(LColor(1, 1, 1, 1));
    entry_text_.set_shadow(LColor(0, 0, 0, 1));
    entry_text_.set_align(TextProperties::A_left);

    auto entry_options = std::make_shared<rppanda::DirectEntry::Options>();
    entry_options->initial_text = "Text Entry";
    entry_options->command = std::bind(&MainUI::entry_on_changed, this, std::placeholders::_1);
    entry_options->pos = LVecBase3(0.8f, 0, -0.32f);
    entry_options->scale = LVecBase3(0.04f);
    entry_ = new rppanda::DirectEntry(ui_root_, entry_options);
}

MainUI::~MainUI()
{
    ui_root_.remove_node();
}

bool MainUI::is_hidden() const
{
    return ui_root_.is_hidden();
}

void MainUI::show()
{
    ui_root_.show();
}

void MainUI::hide()
{
    ui_root_.hide();
}

void MainUI::button_on_clicked()
{
    static size_t count = 0;
    button_text_.set_text("Button Pressed: " + std::to_string(++count));
}

void MainUI::slider_on_changed()
{
    pipeline_.get_daytime_mgr()->set_time(slider_->get_value());
}

void MainUI::checkbox_on_clicked(bool status)
{
    if (status)
        checkbox_text_.set_text("CheckBox Status: On");
    else
        checkbox_text_.set_text("CheckBox Status: Off");
}

void MainUI::entry_on_changed(const std::string& text)
{
    entry_text_.set_text("Entry: " + text);
}
