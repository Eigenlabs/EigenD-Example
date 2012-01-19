/*
 Copyright 2012 Eigenlabs Ltd.  http://www.eigenlabs.com

 This file is part of EigenD.

 EigenD is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 EigenD is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with EigenD.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <piw/piw_cfilter.h>
#include <piw/piw_data.h>

#include "example.h"

#define IN_VOLUME 1
#define IN_FREQUENCY 2
#define IN_MASK SIG2(IN_VOLUME,IN_FREQUENCY)

#define OUT_AUDIO 1
#define OUT_MASK SIG1(OUT_AUDIO)

namespace
{
    struct example_func_t: piw::cfilterfunc_t
    {
        example_func_t(const piw::data_t &path) : path_(path)
        {
            pic::logmsg() << "create " << path;
        }

        bool cfilterfunc_start(piw::cfilterenv_t *env, const piw::data_nb_t &id)
        {
            pic::logmsg() << "start " << id;

            id_ = id;

            env->cfilterenv_reset(IN_VOLUME, id.time());
            env->cfilterenv_reset(IN_FREQUENCY, id.time());

            return true;
        }

        bool cfilterfunc_process(piw::cfilterenv_t *env, unsigned long long from, unsigned long long to, unsigned long samplerate, unsigned buffersize)
        {
            float *buffer_out, *scalar;

            piw::data_nb_t audio_out = piw::makenorm_nb(to, buffersize, &buffer_out, &scalar);
            memset(buffer_out, 0, buffersize*sizeof(float));
            *scalar = 0;

            unsigned signal;
            piw::data_nb_t value;
            while(env->cfilterenv_next(signal, value, to))
            {
                switch(signal)
                {
                    // process the signal values here and change
                    // the buffer_out to generate appropriate audio
                }
            }

            env->cfilterenv_output(OUT_AUDIO, audio_out);

            return true;
        }

        bool cfilterfunc_end(piw::cfilterenv_t *env, unsigned long long to)
        {
            pic::logmsg() << "end " << id_;

            return false;
        }

        const piw::data_t path_;

        piw::data_nb_t id_;
    };
}

namespace cfiltertemplate
{
    struct example_t::impl_t: piw::cfilterctl_t, piw::cfilter_t
    {
        impl_t(const piw::cookie_t &output, piw::clockdomain_ctl_t *domain) : cfilter_t(this, output, domain) {}
        piw::cfilterfunc_t *cfilterctl_create(const piw::data_t &path) { return new example_func_t(path); }
        unsigned long long cfilterctl_thru() { return 0; }
        unsigned long long cfilterctl_inputs() { return IN_MASK; }
        unsigned long long cfilterctl_outputs() { return OUT_MASK; }
    };
}

cfiltertemplate::example_t::example_t(const piw::cookie_t &output, piw::clockdomain_ctl_t *domain) : impl_(new impl_t(output, domain)) {}
piw::cookie_t cfiltertemplate::example_t::cookie() { return impl_->cookie(); }
cfiltertemplate::example_t::~example_t() { delete impl_; }

