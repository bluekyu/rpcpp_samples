/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2016, Center of human-centered interaction for coexistence.
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

#include "plugin.hpp"

#include <boost/dll/alias.hpp>

#include "sample_stage.h"

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
BOOST_SYMBOL_EXPORT const rpcore::BasePlugin::PluginInfo plugin_info = {
    "others",                       // category
    PLUGIN_ID_STRING,               // ID
    "SamplePlugin",                 // name
    "Name <email@email.com>",       // author
    "0.1",                          // version

    "A plugin to use sample."       // description
};

}

static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline& pipeline)
{
    return std::make_shared<Plugin>(pipeline);
}
BOOST_DLL_ALIAS(::create_plugin, create_plugin)

// ************************************************************************************************

struct Plugin::Impl
{
    static RequrieType require_plugins_;

    std::shared_ptr<SampleStage> stage_;
};

Plugin::RequrieType Plugin::Impl::require_plugins_;

// ************************************************************************************************

Plugin::Plugin(rpcore::RenderPipeline& pipeline): rpcore::BasePlugin(pipeline, plugin_info), impl_(new Impl)
{
}

Plugin::RequrieType& Plugin::get_required_plugins(void) const
{
    return impl_->require_plugins_;
}

void Plugin::on_stage_setup(void)
{
    impl_->stage_ = std::make_shared<SampleStage>(pipeline_);
    add_stage(impl_->stage_);
}
