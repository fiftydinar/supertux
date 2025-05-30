//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "badguy/viciousivy.hpp"

#include <math.h>

#include "math/random.hpp"
#include "object/sprite_particle.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/sector.hpp"

ViciousIvy::ViciousIvy(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/vicious_ivy/vicious_ivy.sprite", "left", "right"),
  m_fall_speed()
{
  parse_type(reader);
}

ViciousIvy::ViciousIvy(const Vector& pos, Direction d) :
  WalkingBadguy(pos, d, "images/creatures/vicious_ivy/vicious_ivy.sprite", "left", "right"),
  m_fall_speed()
{
}

GameObjectTypes
ViciousIvy::get_types() const
{
  return {
    { "normal", _("Normal") },
    { "corrupted", _("Corrupted") }
  };
}

std::string
ViciousIvy::get_default_sprite_name() const
{
  switch (m_type)
  {
    case CORRUPTED:
      return "images/creatures/vicious_ivy/corrupted/rotten_ivy.sprite";
    default:
      return m_default_sprite_name;
  }
}

void
ViciousIvy::on_type_change(int old_type)
{
  MovingSprite::on_type_change(old_type);

  switch (m_type)
  {
    case NORMAL:
      walk_speed = 80.f;
      m_fall_speed = 35.f;
      break;
    case CORRUPTED:
      walk_speed = 70.f;
      m_fall_speed = 80.f;
      break;
    default:
      break;
  }
}

bool
ViciousIvy::is_freezable() const
{
  return true;
}

void
ViciousIvy::active_update(float dt_sec)
{
  WalkingBadguy::active_update(dt_sec);
  if (!m_frozen && !m_ignited)
  {
    Rectf floatbox = get_bbox();
    floatbox.set_bottom(get_bbox().get_bottom() + 8.f);

    const bool ignore_unisolid = m_physic.get_velocity_y() < 0.0f;
    bool float_here = (Sector::get().is_free_of_statics(floatbox, nullptr, ignore_unisolid));

    bool in_water = !Sector::get().is_free_of_tiles(get_bbox(), true, Tile::WATER);

    Rectf watertopbox = get_bbox();
    watertopbox.set_top(get_bbox().get_bottom() - get_bbox().get_height()/3.f);
    Rectf wateroutbox = get_bbox();
    wateroutbox.set_bottom(get_bbox().get_top() + (2*get_bbox().get_height()/3.f));

    bool on_top_of_water = (!Sector::get().is_free_of_tiles(watertopbox, true, Tile::WATER) &&
      Sector::get().is_free_of_tiles(wateroutbox, true, Tile::WATER));

    if (in_water)
    {
      if (!on_top_of_water)
      {
        m_physic.set_acceleration_y(-350.f);
        m_physic.set_gravity_modifier(-10.f);
      }
      else
      {
        m_col.set_movement(Vector(m_col.get_movement().x, 0.f));
        m_physic.set_velocity_y(0.f);
        m_physic.set_acceleration_y(0.f);
        m_physic.set_gravity_modifier(0.f);
      }
    }

    if (!float_here) {
      set_action(m_dir);
    } else {
      set_action("float", m_dir);
      if (m_physic.get_velocity_y() >= m_fall_speed && !in_water) {
        m_physic.set_velocity_y(m_fall_speed);
      }
    }
  }
}

bool
ViciousIvy::collision_squished(MovingObject& object)
{
  if (m_frozen)
    return WalkingBadguy::collision_squished(object);

  set_action("squished", m_dir);
  // Spawn death particles.
  spawn_explosion_sprites(3, "images/particles/viciousivy.sprite");
  kill_squished(object);
  return true;
}

/* EOF */
