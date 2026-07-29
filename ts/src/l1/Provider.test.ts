// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { describe, it } from 'node:test';
import assert from 'node:assert/strict';

import { Provider } from './Provider';
import { Converter } from './Converter';
import { Chain } from './Chain';
import { Parameters } from './Parameters';

class UpperCase extends Provider<string, string> {
    resolve(value: string, params: Parameters): string {
        return value.toUpperCase();
    }
}

class NonEmptyFilter extends Provider<string, string | null> {
    accepts(value: string): boolean {
        return value.length > 0;
    }

    resolve(value: string, params: Parameters): string {
        return value;
    }
}

class PrefixedConverter extends Provider<string, string> {
    constructor(private readonly prefix: string) {
        super();
    }

    resolve(value: string, params: Parameters): string {
        return `${this.prefix}${value}`;
    }
}

describe('Provider', () => {
    it('resolve() delegates to the concrete subclass', () => {
        const p = new UpperCase();
        assert.equal(p.resolve('hello', {}), 'HELLO');
    });

    it('is assignable to Converter', () => {
        const p: Converter<string, string> = new UpperCase();
        assert.equal(p.resolve('world', {}), 'WORLD');
    });

    it('optional accepts() defined on a subclass is present and callable', () => {
        const p = new NonEmptyFilter();
        assert.equal(p.accepts?.('hello'), true);
        assert.equal(p.accepts?.(''), false);
    });

    it('optional accepts() is absent when not defined on the subclass', () => {
        const p: Converter<string, string> = new UpperCase();
        assert.equal(p.accepts, undefined);
    });

    it('can be installed in a Chain and participates in normal chain resolution', () => {
        const chain = new Chain<string, string | null>(null);
        chain.install(1, 'non-empty', new NonEmptyFilter());
        assert.equal(chain.resolve('hello', {}), 'hello');
        assert.equal(chain.resolve('', {}), null);
    });

    it('constructor arguments work as expected in subclasses', () => {
        const p = new PrefixedConverter('Mr. ');
        assert.equal(p.resolve('Smith', {}), 'Mr. Smith');
    });
});
