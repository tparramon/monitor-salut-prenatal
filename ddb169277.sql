-- phpMyAdmin SQL Dump
-- version 5.1.0
-- https://www.phpmyadmin.net/
--
-- Servidor: localhost
-- Temps de generació: 13-05-2021 a les 12:30:13
-- Versió del servidor: 10.4.18-MariaDB
-- Versió de PHP: 8.0.3

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Base de dades: `ddb169277`
--

-- --------------------------------------------------------

--
-- Estructura de la taula `adx`
--

CREATE TABLE `adx` (
  `id` int(11) NOT NULL,
  `adx_x` int(11) NOT NULL,
  `adx_y` int(11) NOT NULL,
  `adx_z` int(11) NOT NULL,
  `data` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Estructura de la taula `bpm`
--

CREATE TABLE `bpm` (
  `id` int(11) NOT NULL,
  `valor` int(11) NOT NULL,
  `data` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Estructura de la taula `fsr`
--

CREATE TABLE `fsr` (
  `id` int(11) NOT NULL,
  `valor` int(11) NOT NULL,
  `data` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Estructura de la taula `ntc`
--

CREATE TABLE `ntc` (
  `id` int(11) NOT NULL,
  `valor` int(11) NOT NULL,
  `data` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Estructura de la taula `spo2`
--

CREATE TABLE `spo2` (
  `id` int(11) NOT NULL,
  `valor` int(11) NOT NULL,
  `data` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Índexs per a les taules bolcades
--

--
-- Índexs per a la taula `adx`
--
ALTER TABLE `adx`
  ADD PRIMARY KEY (`id`);

--
-- Índexs per a la taula `bpm`
--
ALTER TABLE `bpm`
  ADD PRIMARY KEY (`id`);

--
-- Índexs per a la taula `fsr`
--
ALTER TABLE `fsr`
  ADD PRIMARY KEY (`id`);

--
-- Índexs per a la taula `ntc`
--
ALTER TABLE `ntc`
  ADD PRIMARY KEY (`id`);

--
-- Índexs per a la taula `spo2`
--
ALTER TABLE `spo2`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT per les taules bolcades
--

--
-- AUTO_INCREMENT per la taula `adx`
--
ALTER TABLE `adx`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT per la taula `bpm`
--
ALTER TABLE `bpm`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT per la taula `fsr`
--
ALTER TABLE `fsr`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT per la taula `ntc`
--
ALTER TABLE `ntc`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT per la taula `spo2`
--
ALTER TABLE `spo2`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
