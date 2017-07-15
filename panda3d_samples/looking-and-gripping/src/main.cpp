/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017, Younguk Kim (bluekyu)
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

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/gui/onscreen_text.hpp>

int main(int argc, char* argv[])
{
    rppanda::ShowBase base(argc, argv);

    rppanda::OnscreenText::Parameters params;
    params.text = "Panda3D: Tutorial - Joint Manipulation";
    params.fg = LColor(1, 1, 1, 1);
    params.parent = rppanda::ShowBase::get_global_ptr()->get_aspect_2d().find("a2d_bottom_right");
    params.align = TextProperties::A_right;
    params.pos = LVecBase2(-0.1f, 0.1f);
    params.shadow = LColor(0, 0, 0, 0.5f);
    params.scale = 0.08f;
    rppanda::OnscreenText title_(params);

    base.run();

    return 0;
}
