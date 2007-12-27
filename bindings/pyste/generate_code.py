#! /usr/bin/python
# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
# This version much modified by Michael Donaghy

import os
import glob
import pygccxml.parser
from pyplusplus import module_builder

c=pygccxml.parser.directory_cache_t()

#h=glob.glob("../../libsakusen/*.h")

h=["../../libsakusen/comms/socket.h","../../libsakusen/comms/messagedata.h","../../libsakusen/comms/unixdatagramlisteningsocket.h"]

a=[os.path.abspath(x) for x in h]
mb=module_builder.module_builder_t(files=a,include_paths=["../../libsakusen","../../libsakusen/comms"],cache=c,cflags="-fxml-start=sakusen")

mb.member_operators(name="operator>>",header_file=os.path.abspath("../../libsakusen/iarchive.h")).exclude()
#the template metaprogramming, she cannae take it captain. Yes, the argument types do need to be specified like this, as far as I can tell
mb.constructor(name="Message",arg_types=["::sakusen::IArchive &","::sakusen::PlayerId const","::boost::shared_ptr<sakusen::ResourceInterface> const &"]).exclude()

#mb.class_("IArchive").exclude()
mb.namespace("sakusen").namespace("comms").exclude()

mb.build_code_creator(module_name='libsakusen')

mb.write_module(os.path.join(os.path.abspath('.'),'libsakusen.cpp'))

mb=module_builder.module_builder_t(files=a,include_paths=["../../libsakusen","../../libsakusen/comms"],cache=c,cflags="-fxml-start=sakusen")

mb.member_operators(name="operator>>",header_file=os.path.abspath("../../libsakusen/iarchive.h")).exclude()
#the template metaprogramming, she cannae take it captain. Yes, the argument types do need to be specified like this, as far as I can tell
mb.constructor(name="Message",arg_types=["::sakusen::IArchive &","::sakusen::PlayerId const","::boost::shared_ptr<sakusen::ResourceInterface> const &"]).exclude()
mb.member_function(name="accept",header_file=os.path.abspath("../../libsakusen/comms/unixdatagramsocket.h")).exclude()

#mb.class_("IArchive").exclude()
mb.namespace("sakusen").already_exposed = True

mb.build_code_creator(module_name='libsakusencomms')

mb.write_module(os.path.join(os.path.abspath('.'),'libsakusencomms.cpp'))

