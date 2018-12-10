/****************************************************************************

  This file is part of the Webstella protocols exchange (Weprex) software.

  Copyright (C) 2018 Oleg Malyavkin.
  Contact: weprexsoft@gmail.com

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/

#ifndef WSPARAMETERSHASH_H
#define WSPARAMETERSHASH_H

#include <QtCore>
#include <memory>
#include <new>
#include <map>
#include <functional>

template <class T> class WSParametersHash {

public:
	WSParametersHash(std::function<void(quint32)> callbackFunction) :
		m_callbackFunction(callbackFunction),
		m_lastId(0)
	{}

	void append(quint32 id, T *param) {
		m_params[id] = std::unique_ptr<T>(param);
		if (m_callbackFunction != nullptr) {
			m_callbackFunction(id);
		}
		if (id > m_lastId) {
			m_lastId = id;
		}
	}
	
	void remove(quint32 id) {
		m_params.erase(id);
		if (m_callbackFunction != nullptr) {
			m_callbackFunction(id);
		}
	}
	
	T* get(quint32 id) {
		return (m_params[id]).get();
	}

	bool contains(quint32 id) {
		return m_params.find(id) != m_params.end();
	}
	
	typename std::map<quint32, std::unique_ptr<T> >::iterator begin() {
		return m_params.begin();
	}
	
	typename std::map<quint32, std::unique_ptr<T> >::iterator end() {
		return m_params.end();
	}

	quint32 lastId() const {
		return m_lastId;
	}

private:
	std::map<quint32, std::unique_ptr<T> > m_params;
	std::function<void(quint32)> m_callbackFunction;
	quint32 m_lastId;
};

#endif // WSPARAMETERSHASH_H
