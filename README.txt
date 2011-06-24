UCNSIM - A Monte-Carlo Simulation of Ultra-Cold Neutrons 
========================================================

Authors:      Matthew Rásó-Barnett
Date:         June 2011

Overview
========

This is a collection of programs designed to produce simulations of ultra-cold neutrons (UCN) through user-defined, custom experimental geometries. 

There are a number of good, well supported simulation packages designed for high-energy particle physics in existence today, however the same cannot be said for ultra-low energy particle physics such as the processes involved in the storage of UCN (particularly motion under gravity). There are a few small, domain-specific simulations that different experiments have developed for their own specific purposes, however there are no (to the author's knowledge) widely used packages, and certainly nothing with a community and level of support to anywhere near standards enjoyed by the high-energy physics community.

This simulation package is designed as a tentative first step towards solving part of that problem, by being a more general purpose simulation for ultra-cold neutron physics than is typically available at present in this field.

Some of the physical processes that are currently included are:
-- Neutron motion under gravity
-- Interaction with neutron-guide boundaries. 
-  Collisions with the boundaries of the geometry are calculated analytically   for the following shapes: Cuboid, Cylinder and Boolean compositions of these (union, subtraction and intersection). Further shapes are planned to be supported in future.
-  Neutron capture by nuclei of the material boundaries, and inelastic up-scattering.
-- Neutron spin interaction with arbitrary magnetic fields
-  Magnetic fields can either be specified as a analytic function or any magnetic field maps can be imported to be used by the simulation. The simulation will handle all interpolation of field vertices, so the imported field maps do not require any specific 'structure' for the vertices (like a rectangular grid).
-  Electric fields can be defined in exactly the same way as magnetic fields (i.e: analytic function or field map). The motional magnetic field the neutron experiences as it travels through the electric field (v x E effect) is included.
-- Neutron beta-decay

Availability
============

The current stable version of UCNSIM is available from GitHub http://github.com/mjrasobarnett/ucnsim

If you wish to follow the development you can download the source-code via git:

  git clone git://github.com/mjrasobarnett/ucnsim

Installation
============

See INSTALL.txt

License
=======

See LICENSE.txt


